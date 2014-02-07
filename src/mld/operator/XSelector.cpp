/****************************************************************************
**
** Copyright (C) 2013 EPFL-LTS2
** Contact: Kirell Benzi (first.last@epfl.ch)
**
** This file is part of MLD.
**
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.md included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements
** will be met: http://www.gnu.org/licenses/
**
****************************************************************************/

#include <numeric>  // std::accumulate
#include <dex/gdb/Graph.h>
#include <dex/gdb/Objects.h>
#include <dex/gdb/ObjectsIterator.h>

#include "mld/Graph_types.h"
#include "mld/operator/XSelector.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace dex::gdb;

namespace {

} // end namespace anonymous

XSelector::XSelector( Graph* g )
    : AbstractMultiSelector(g)
{
}

XSelector::~XSelector()
{
}

bool XSelector::rankNodes( const Layer& layer )
{
    m_scores.clear();
    m_lid = layer.id();
    ObjectsPtr nodes(m_dao->getAllNodeIds(layer));
    // Create an empty but valid ObjectPtr
    m_rootNodes.reset(nodes->Copy());
    m_rootNodes->Clear();
    // Iterate through each node and calculate score
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() ) {
        auto nid = it->Next();
        // Add values in the mutable priority queue
        m_scores.push(nid, calcScore(nid));
    }
    return true;
}

bool XSelector::hasNext()
{
    return !m_scores.empty();
}

SuperNode XSelector::next()
{
    if( hasNext() ) {
        SuperNode n = m_dao->getNode(m_scores.front_value());
        // Add to root node set
        m_rootNodes->Add(n.id());
        // Remove item
        m_scores.pop();
        return n;
    }
    else {
        return SuperNode();
    }
}

ObjectsPtr XSelector::unflaggedNeighbors( oid_t node )
{
    ObjectsPtr neighbors(m_dao->graph()->Neighbors(node, m_dao->superNodeType(), Any));
    // Return only neighbors not flagged
    neighbors->Difference(m_flagged.get());
    return neighbors;
}

bool XSelector::flagNode( const SuperNode& root )
{
    m_flagged->Add(root.id());

    ObjectsPtr unflagged(unflaggedNeighbors(root.id()));
    if( unflagged->Count() == 0 )
        return true;

    m_flagged->Union(unflagged.get());
    // Remove from selection queue
    removeCandidates(unflagged);

    // Keep only unflagged neighbor nodes
    ObjectsPtr neighbors(m_dao->graph()->Neighbors(unflagged.get(), m_dao->superNodeType(), Any));
    neighbors = unflaggedNodes(neighbors);
    return updateScore(neighbors);
}

bool XSelector::isFlagged( oid_t snid )
{
    return m_flagged->Exists(snid);
}

ObjectsPtr XSelector::flaggedNodes( const ObjectsPtr& input )
{
    return ObjectsPtr(Objects::CombineIntersection(input.get(), m_flagged.get()));
}

ObjectsPtr XSelector::unflaggedNodes( const ObjectsPtr& input )
{
    return ObjectsPtr(Objects::CombineDifference(input.get(), m_flagged.get()));
}

double XSelector::calcScore( oid_t snid , bool withFlagged )
{
    double iWeight = inScore(snid, withFlagged);
    double oWeight = outScore(snid, withFlagged);
    double nWeight = nodeScore(snid, withFlagged);
    // TODO take into account root WEIGHT
    return iWeight / (oWeight * nWeight);
}

bool XSelector::updateScore( const ObjectsPtr& input )
{
    ObjectsIt it(input->Iterator());
    while( it->HasNext() ) {
       auto current = it->Next();
       // Take into acocunt flagged nodes
       m_scores.update(current, calcScore(current, true));
    }
    return true;
}

void XSelector::removeCandidates( const ObjectsPtr& input )
{
    if( !input )
        return;
    // Remove candidates nodes from selection
    ObjectsIt it(input->Iterator());
    while( it->HasNext() ) {
        m_scores.erase(it->Next());
    }
}

ObjectsPtr XSelector::inEdges( oid_t root, bool withFlagged )
{
    return inOrOutEdges(true, root, withFlagged);
}

ObjectsPtr XSelector::outEdges( oid_t root, bool withFlagged )
{
    return inOrOutEdges(false, root, withFlagged);
}

double XSelector::getEdgeWeight( const ObjectsPtr& edgeOids )
{
    std::vector<HLink> hlinks = m_dao->getHLink(edgeOids);
    if( hlinks.empty() ) {
        LOG(logERROR) << "XSelector::edgeWeight << cannot retrieve hlinks or no hlinks";
        return 1.0;
    }
    double total = 0.0;
    for( auto& hlink: hlinks ) {
        total += hlink.weight();
    }
    return total;
}

/**
 * @brief In score = traverse_edge_weight / in_edge_weight
 */
double XSelector::inScore( oid_t node, bool withFlagged )
{
    ObjectsPtr traverseEdges;
    if( !withFlagged ) {
        traverseEdges.reset(m_dao->graph()->Explode(node, m_dao->superNodeType(), Any));
    }
    else { // Remove flagged nodes
        ObjectsPtr targetSet(unflaggedNeighbors(node));
        traverseEdges.reset(targetSet->Copy());
        traverseEdges->Clear();
        edgeRetriever(traverseEdges, node, targetSet);
    }

    double travWeight = getEdgeWeight(traverseEdges);
    double inWeight = getEdgeWeight(inEdges(node, withFlagged));
    return travWeight / inWeight;
}

double XSelector::outScore( oid_t node, bool withFlagged )
{
    auto out = outEdges(node, withFlagged);
    double outWeight = getEdgeWeight(out);
    return outWeight / double(out->Count());
}

double XSelector::nodeScore( oid_t node, bool withFlagged )
{
    ObjectsPtr neighbors;
    if( !withFlagged )
        neighbors.reset(m_dao->graph()->Neighbors(node, m_dao->superNodeType(), Any));
    else
        neighbors = unflaggedNeighbors(node);
    auto nodes = m_dao->getNode(neighbors);
    if( !nodes.empty() ) {
        LOG(logWARNING) << "XSelector::nodeScore: node has no neighbors (disconnected graph)";
        return 1.0;
    }
    double total = 0.0;
    for( auto& node: nodes ) {
        total += node.weight();
    }
    // N = mean(node_w) / #node
    size_t count = nodes.size();
    return total / double(count * count);
}


ObjectsPtr XSelector::inOrOutEdges( bool inEdges, oid_t root, bool withFlagged )
{
    ObjectsPtr neighbors;
    if( !withFlagged )
        neighbors.reset(m_dao->graph()->Neighbors(root, m_dao->superNodeType(), Any));
    else
        neighbors = unflaggedNeighbors(root);
    // Create empty edge set
    ObjectsPtr edgeSet(neighbors->Copy());
    edgeSet->Clear();

    ObjectsIt it(neighbors->Iterator());
    while( it->HasNext() ) {
        auto source = it->Next();
        ObjectsPtr hop2;
        if( !withFlagged ) {
            hop2.reset(m_dao->graph()->Neighbors(source, m_dao->superNodeType(), Any));
        }
        else {
            hop2 = unflaggedNeighbors(source);
        }

        if( inEdges ) {
            // Keep only neighbors within 1 hop radius from root
            hop2->Intersection(neighbors.get());
        }
        else {
            hop2->Difference(neighbors.get());
        }
        // Retrieve edges
        edgeRetriever(edgeSet, source, hop2);
    }
    return edgeSet;
}

void XSelector::edgeRetriever( ObjectsPtr& edgeSet, oid_t source, const ObjectsPtr& targetSet )
{
    ObjectsIt it(targetSet->Iterator());
    while( it->HasNext() ) {
        auto target = it->Next();
        // Retrieve edge oid
        oid_t eid = m_dao->graph()->FindEdge(m_dao->hlinkType(), source, target);
#ifdef MLD_SAFE
        if( eid == Objects::InvalidOID ) {
            LOG(logERROR) << "XSelector::edgeRetriever cannot retrieve eid between 2 nodes";
            continue;
        }
#endif
        edgeSet->Add(eid);
    }
}
