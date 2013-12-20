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

void edgeRetriever( ObjectsPtr& edgeSet, oid_t source, const ObjectsPtr& targetSet, MLGDao* dao )
{
    ObjectsIt it(targetSet->Iterator());
    while( it->HasNext() ) {
        auto target = it->Next();
        // Retrieve edge oid
        oid_t eid = dao->graph()->FindEdge(dao->hlinkType(), source, target);
#ifdef MLD_SAFE
        if( eid == Objects::InvalidOID ) {
            LOG(logERROR) << "XSelector::edgeRetriever cannot retrieve eid between 2 nodes";
            continue;
        }
#endif
        edgeSet->Add(eid);
    }
}

/**
 * @brief inOrOutEdges, retrieve all edges within 1hop radius or all out edges from 1 hop
 * radius
 * @param inEdges, if true get inedges, else out edges
 * @param root node
 * @param dao
 * @return edge set
 */
ObjectsPtr inOrOutEdges( bool inEdges, oid_t root, MLGDao* dao )
{
    ObjectsPtr neighbors(dao->graph()->Neighbors(root, dao->superNodeType(), Any));
    // Create empty edge set
    ObjectsPtr edgeSet(neighbors->Copy());
    edgeSet->Clear();

    ObjectsIt it(neighbors->Iterator());
    while( it->HasNext() ) {
        auto source = it->Next();
        ObjectsPtr hop2(dao->graph()->Neighbors(source, dao->superNodeType(), Any));
        if( inEdges ) {
            // Keep only neighbors within 1 hop radius from root
            hop2->Intersection(neighbors.get());
        }
        else {
            hop2->Difference(neighbors.get());
        }
        // Retrieve edges
        edgeRetriever(edgeSet, source, hop2, dao);
    }
    return edgeSet;
}

ObjectsPtr inEdges( oid_t root, MLGDao* dao )
{
    return inOrOutEdges(true, root, dao);
}

ObjectsPtr outEdges( oid_t root, MLGDao* dao )
{
    return inOrOutEdges(false, root, dao);
}

double getEdgeWeight( const ObjectsPtr& edgeOids, MLGDao* dao )
{
    std::vector<HLink> hlinks = dao->getHLink(edgeOids);
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
double inScore( oid_t node, MLGDao* dao )
{
    ObjectsPtr traverseEdges(dao->graph()->Explode(node, dao->superNodeType(), Any));
    double travWeight = getEdgeWeight(traverseEdges, dao);
    double inWeight = getEdgeWeight(inEdges(node, dao), dao);
    return travWeight / inWeight;
}

double outScore( oid_t node, MLGDao* dao )
{
    auto out = outEdges(node, dao);
    double outWeight = getEdgeWeight(out, dao);
    return outWeight / double(out->Count());
}

double nodeScore( oid_t node, MLGDao* dao )
{
    ObjectsPtr neighbors(dao->graph()->Neighbors(node, dao->superNodeType(), Any));
    auto nodes = dao->getNode(neighbors);
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

ObjectsPtr XSelector::neighbors( const SuperNode& node )
{
    ObjectsPtr neighbors(m_dao->graph()->Neighbors(node.id(), m_dao->superNodeType(), Any));
    // Return only neighbors not flagged
    neighbors->Difference(m_flagged.get());
    return neighbors;
}

bool XSelector::flagNode( const SuperNode& root )
{
    ObjectsPtr hop1(m_dao->graph()->Neighbors(root.id(), m_dao->superNodeType(), Any));
    ObjectsPtr hop2(m_dao->graph()->Neighbors(hop1.get(), m_dao->superNodeType(), Any));

    // Keep only 2-hop flagged nodes
    hop2->Intersection(m_flagged.get());

    // Get root parent Node
    auto parents = m_dao->getParentNodes(root.id());
    if( parents.empty() ) {
        LOG(logERROR) << " XSelector::flagNode: root node has no parents";
        return false;
    }
    SuperNode rootTop = parents.at(0);

    // For each 2hop flagged node, get root node, get Vlink and
    // hlink rootTop and 2hopTop node
    ObjectsIt it(hop2->Iterator());
    while( it->HasNext() ) {
        auto current = it->Next();
        ObjectsPtr currentNeighbors(m_dao->graph()->Neighbors(current, m_dao->superNodeType(), Any));
        // Get root node for current node
        ObjectsPtr rootSet(Objects::CombineIntersection(currentNeighbors.get(), m_rootNodes.get()));
        if( rootSet->Count() != 1 ) {
            LOG(logERROR) << " XSelector::flagNode: 2hop node has "
                          << rootSet->Count() << "root node(s)";
            return false;
        }
        // Get root node
        auto curRoot = rootSet->Any();
        parents = m_dao->getParentNodes(curRoot);
        if( parents.empty() ) {
            LOG(logERROR) << "XSelector::flagNode: 2hop root node has no parents";
            return false;
        }
        SuperNode hop3rootTop = parents.at(0);
        updateOrCreateHLink(current, currentNeighbors, hop1, rootTop, hop3rootTop);
    }
    // Flag node + neighbors
    m_flagged->Union(hop1.get());
    m_flagged->Add(root.id());

    // TODO
    // update 2hop scores maybe not needed
    return true;
}

bool XSelector::isFlagged( oid_t snid )
{
    return m_flagged->Exists(snid);
}

double XSelector::calcScore( oid_t snid )
{
    double iWeight = inScore(snid, m_dao.get());
    double oWeight = outScore(snid, m_dao.get());
    double nWeight = nodeScore(snid, m_dao.get());
    // TODO take into account root WEIGHT
    return iWeight / (oWeight * nWeight);
}

bool XSelector::updateOrCreateHLink( oid_t root2HopCurrentNode,
                                     ObjectsPtr& root2HopCurrentNeighbors,
                                     ObjectsPtr& rootNeighbors,
                                     const SuperNode& rootTop,
                                     const SuperNode& root3HopTop )
{
    // Retrieve edges between root neighbors and 2-hop current neighbors, at least 1 element
    ObjectsPtr hop1Targets(Objects::CombineIntersection(root2HopCurrentNeighbors.get(), rootNeighbors.get()));
#ifdef MLD_SAFE
    if( hop1Targets->Count() == 0 ) {
        LOG(logERROR) << " XSelector::updateOrCreateHLink: cannot retrieve 1-hop node from 2hop neighbors";
        return false;
    }
#endif
    double totalWeight = 0.0;
    // Iterate through all edges between root neighbors and 2hop current node
    ObjectsIt it(hop1Targets->Iterator());
    while( it->HasNext() ) {
        HLink l = m_dao->getHLink(it->Next(), root2HopCurrentNode);
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << " XSelector::updateOrCreateHLink: Invalid retrieved HLink";
            return false;
        }
#endif
        // Add each edge weight
        totalWeight += l.weight();
    }
    // Createor update final HLink in top layer between 2 top root supernodes
    HLink topLink = m_dao->getHLink(rootTop.id(), root3HopTop.id());
    if( topLink.id() == Objects::InvalidOID ) {
        HLink res = m_dao->addHLink(rootTop.id(), root3HopTop.id(), totalWeight);
#ifdef MLD_SAFE
        if( res.id() == Objects::InvalidOID ) {
            LOG(logERROR) << " XSelector::updateOrCreateHLink: Failed to HLink top root nodes";
            return false;
        }
#endif
    }
    else {
        topLink.setWeight(topLink.weight() + totalWeight);
        // Update weight
        bool ok = m_dao->updateHLink(topLink.id(), topLink.weight());
#ifdef MLD_SAFE
        if( !ok ) {
            LOG(logERROR) << " XSelector::updateOrCreateHLink: Failed to update top root nodes HLINK";
            return false;
        }
#endif
    }
    return true;
}
