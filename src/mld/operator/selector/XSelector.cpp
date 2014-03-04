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

#include <sparksee/gdb/Graph.h>
#include <sparksee/gdb/Objects.h>
#include <sparksee/gdb/ObjectsIterator.h>

#include "mld/operator/selector/XSelector.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

namespace {

} // end namespace anonymous

XSelector::XSelector( Graph* g )
    : NeighborSelector(g)
{
}

XSelector::~XSelector()
{
}

double XSelector::calcScore( oid_t snid )
{
    if( snid == Objects::InvalidOID ) {
        LOG(logERROR) << "XSelector::calcScore invalid id";
        return 0.0;
    }
    double r = rootCentralityScore(snid);
    double h = twoHopHubAffinityScore(snid);
    double g = gravityScore(snid);
    return r / (g * h);
}

void XSelector::setCurrentBestNeighbors()
{
    m_curNeighbors = getNeighbors(m_current);
    if( !m_curNeighbors ) {
        LOG(logERROR) << "XSelector::setCurrentBestNeighbors m_curNeighbors is null";
        return;
    }
}

ObjectsPtr XSelector::inEdges( oid_t root )
{
    return inOrOutEdges(true, root);
}

ObjectsPtr XSelector::outEdges( oid_t root )
{
    return inOrOutEdges(false, root);
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

double XSelector::rootCentralityScore( oid_t node )
{
    double travWeight = 0.0;
    double inWeight = 1.0;

    ObjectsPtr traverseEdges;
    if( m_hasMemory ) {
        traverseEdges = m_dao->newObjectsPtr();
        ObjectsPtr targetSet(getNeighbors(node));
        edgeRetriever(traverseEdges, node, targetSet);
    }
    else {
        traverseEdges.reset(m_dao->graph()->Explode(node, m_dao->hlinkType(), Any));
    }

    // Only direct edges from root to neighbors
    if( traverseEdges->Count() > 0 )
        travWeight = getEdgeWeight(traverseEdges);

    // 1 hop radius edges
    ObjectsPtr inE = inEdges(node);
    if( inE->Count() > 0 )
        inWeight = getEdgeWeight(inE);

    return travWeight / inWeight;
}

double XSelector::twoHopHubAffinityScore( oid_t node )
{
    auto out = outEdges(node);
    return double(std::max(out->Count(), int64_t(1)));
}

double XSelector::gravityScore( oid_t root )
{
    ObjectsPtr nodeSet = getNeighbors(root);
    nodeSet->Add(root);
    auto nodes = m_dao->getNode(nodeSet);
    double total = 0.0;
    for( auto& node: nodes ) {
        total += node.weight();
    }
    return total;
}

ObjectsPtr XSelector::inOrOutEdges( bool inEdges, oid_t root )
{
    ObjectsPtr edgeSet = m_dao->newObjectsPtr();
    ObjectsPtr neighbors = getNeighbors(root);
    // Root + neighbors
    ObjectsPtr rootSet(neighbors->Copy());
    rootSet->Add(root);


    ObjectsIt it(neighbors->Iterator());
    while( it->HasNext() ) {
        auto source = it->Next();
        ObjectsPtr hop2 = getNeighbors(source);
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
