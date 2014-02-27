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
#include <sparksee/gdb/Graph.h>
#include <sparksee/gdb/Objects.h>
#include <sparksee/gdb/ObjectsIterator.h>

#include "mld/GraphTypes.h"
#include "mld/operator/selector/XSelector.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

namespace {

} // end namespace anonymous

XSelector::XSelector( Graph* g )
    : AbstractMultiSelector(g)
    , m_flagged( m_dao->newObjectsPtr() )
    , m_rootNodes( m_dao->newObjectsPtr() )
{
}

XSelector::~XSelector()
{
}

bool XSelector::rankNodes( const Layer& layer )
{
    // Reset variables
    m_scores.clear();
    m_lid = layer.id();
    m_flagged = m_dao->newObjectsPtr();
    m_rootNodes = m_dao->newObjectsPtr();

    ObjectsPtr nodes(m_dao->getAllNodeIds(layer));
    // Iterate through each node and calculate score
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() ) {
        auto nid = it->Next();
#ifdef MLD_SAFE
        if( nid == Objects::InvalidOID ) {
            LOG(logERROR) << "XSelector::rankNodes invalid node";
            return false;
        }
#endif
        // Add values in the mutable priority queue, no nodes are flagged
        m_scores.push(nid, calcScore(nid, true));
    }
    return true;
}

bool XSelector::hasNext()
{
    return !m_scores.empty();
}

SuperNode XSelector::next( bool popNode )
{
    if( !m_scores.empty() ) {
        oid_t nid = m_scores.front_value();

        if( popNode )
            m_scores.pop();

        return m_dao->getNode(nid);
    }
    else {
        return SuperNode();
    }
}

ObjectsPtr XSelector::getUnflaggedNeighbors( oid_t node )
{
    ObjectsPtr neighbors(m_dao->graph()->Neighbors(node, m_dao->hlinkType(), Any));
    // Return only neighbors not flagged
    neighbors->Difference(m_flagged.get());
    return neighbors;
}

bool XSelector::flagAndUpdateScore( const SuperNode& root, bool removeNeighbors, bool withFlagged )
{   
    // Add to root node set
    m_rootNodes->Add(root.id());
    m_flagged->Add(root.id());

    ObjectsPtr unflagged(getUnflaggedNeighbors(root.id()));
    if( unflagged->Count() == 0 )
        return true;

    if( removeNeighbors ) {
        // Add current root, to be removed from queue
        // if selector is used normally with the next() method
        // root node is already removed from queue
        unflagged->Add(root.id());
        m_flagged->Union(unflagged.get());
        // Remove from selection queue
        removeCandidates(unflagged);
        unflagged->Remove(root.id());
    }

    // Keep only unflagged neighbor nodes
    ObjectsPtr neighbors(m_dao->graph()->Neighbors(unflagged.get(), m_dao->hlinkType(), Any));
    neighbors = getUnflaggedNodesFrom(neighbors);
    return updateScore(neighbors, withFlagged);
}

bool XSelector::isFlagged( oid_t snid )
{
    return m_flagged->Exists(snid);
}

ObjectsPtr XSelector::getFlaggedNodesFrom( const ObjectsPtr& input )
{
    return ObjectsPtr(Objects::CombineIntersection(input.get(), m_flagged.get()));
}

ObjectsPtr XSelector::getUnflaggedNodesFrom( const ObjectsPtr& input )
{
    return ObjectsPtr(Objects::CombineDifference(input.get(), m_flagged.get()));
}

double XSelector::calcScore( oid_t snid , bool withFlagged )
{
    double r = rootCentralityScore(snid, withFlagged);
    double h = twoHopHubAffinityScore(snid, withFlagged);
    double g = gravityScore(snid, withFlagged);
    return r / (g * h);
}

bool XSelector::updateScore( const ObjectsPtr& input, bool withFlagged )
{
    ObjectsIt it(input->Iterator());
    while( it->HasNext() ) {
        auto id = it->Next();
        m_scores.update(id, calcScore(id, withFlagged));
    }
    return true;
}

ObjectsPtr XSelector::getHLinkEnpoints( const SuperNode& root, bool oneHopOnly )
{
    ObjectsPtr hop1(m_dao->graph()->Neighbors(root.id(), m_dao->hlinkType(), Any));
    if( oneHopOnly )
        return getFlaggedNodesFrom(hop1);

    ObjectsPtr hop1flagged(getFlaggedNodesFrom(hop1));
    ObjectsPtr hop1Unflagged(getUnflaggedNodesFrom(hop1));
    ObjectsPtr nodeSet(m_dao->graph()->Neighbors(hop1Unflagged.get(), m_dao->hlinkType(), Any));

    // Keep only unflagged 2-hop's flagged nodes and 1hop flagged nodes
    nodeSet = getFlaggedNodesFrom(nodeSet);
    nodeSet->Union(hop1flagged.get());
    return nodeSet;
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
        LOG(logERROR) << "XSelector::edgeWeight << cannot retrieve hlinks";
        return 0.0;
    }
    double total = 0.0;
    for( auto& hlink: hlinks ) {
        total += hlink.weight();
    }
    return total;
}

double XSelector::rootCentralityScore( oid_t node, bool withFlagged )
{
    double travWeight = 0.0;
    double inWeight = 1.0;

    ObjectsPtr traverseEdges;
    if( withFlagged ) {
        traverseEdges.reset(m_dao->graph()->Explode(node, m_dao->hlinkType(), Any));
    }
    else { // Remove flagged nodes
        traverseEdges = m_dao->newObjectsPtr();
        ObjectsPtr targetSet(getUnflaggedNeighbors(node));
        edgeRetriever(traverseEdges, node, targetSet);
    }

    // Only direct edges from root to neighbors
    if( traverseEdges->Count() > 0 )
        travWeight = getEdgeWeight(traverseEdges);

    // 1 hop radius edges
    ObjectsPtr inE = inEdges(node, withFlagged);
    if( inE->Count() > 0 )
        inWeight = getEdgeWeight(inE);

    return travWeight / inWeight;
}

double XSelector::twoHopHubAffinityScore( oid_t node, bool withFlagged )
{
    auto out = outEdges(node, withFlagged);
    return double(std::max(out->Count(), int64_t(1)));
}

double XSelector::gravityScore( oid_t root, bool withFlagged )
{
    ObjectsPtr nodeSet;
    if( withFlagged )
        nodeSet.reset(m_dao->graph()->Neighbors(root, m_dao->hlinkType(), Any));
    else
        nodeSet = getUnflaggedNeighbors(root);

    nodeSet->Add(root);
    auto nodes = m_dao->getNode(nodeSet);
    double total = 0.0;
    for( auto& node: nodes ) {
        total += node.weight();
    }
    return total;
}


ObjectsPtr XSelector::inOrOutEdges( bool inEdges, oid_t root, bool withFlagged )
{
    ObjectsPtr neighbors;
    if( withFlagged )
        neighbors.reset(m_dao->graph()->Neighbors(root, m_dao->hlinkType(), Any));
    else
        neighbors = getUnflaggedNeighbors(root);

    // Root + neighbors
    ObjectsPtr rootSet(neighbors->Copy());
    rootSet->Add(root);

    ObjectsPtr edgeSet = m_dao->newObjectsPtr();

    ObjectsIt it(neighbors->Iterator());
    while( it->HasNext() ) {
        auto source = it->Next();
        ObjectsPtr hop2;
        if( withFlagged ) {
            hop2.reset(m_dao->graph()->Neighbors(source, m_dao->hlinkType(), Any));
        }
        else {
            hop2 = getUnflaggedNeighbors(source);
        }
        ObjectsPtr nodeSet;
        if( inEdges ) {
            // Keep only neighbors within 1 hop radius from root
            nodeSet.reset(Objects::CombineIntersection(rootSet.get(), hop2.get()));
            edgeRetriever(edgeSet, source, nodeSet);
        }
        else {
            nodeSet.reset(Objects::CombineDifference(hop2.get(), rootSet.get()));
            edgeRetriever(edgeSet, source, nodeSet);
        }
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
