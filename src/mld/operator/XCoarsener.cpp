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

#include <dex/gdb/Graph.h>
#include <dex/gdb/Objects.h>
#include <dex/gdb/ObjectsIterator.h>

#include "mld/dao/MLGDao.h"
#include "mld/operator/XCoarsener.h"
#include "mld/operator/XSelector.h"
#include "mld/operator/MultiAdditiveMerger.h"

using namespace mld;
using namespace dex::gdb;

namespace {

double totalEdgeWeight( oid_t source, const ObjectsPtr& targets, MLGDao* dao )
{
    double totalWeight = 0.0;
    // Iterate through all edges between root neighbors and 2hop current node
    ObjectsIt it(targets->Iterator());
    while( it->HasNext() ) {
        HLink l = dao->getHLink(it->Next(), source);
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << " XSelector::totalEdgeWeight: Invalid retrieved HLink";
            return 0.0;
        }
#endif
        // Add each edge weight
        totalWeight += l.weight();
    }
    return totalWeight;
}

} // end namespace anonymous

XCoarsener::XCoarsener( Graph* g )
    : AbstractOperator(g)
    , m_sel( new XSelector(g) )
    , m_merger( new MultiAdditiveMerger(g) )
{
}

XCoarsener::~XCoarsener()
{
}

bool XCoarsener::preExec()
{
    // Check current layer node count
    Layer current = m_dao->topLayer();
    if( m_dao->getNodeCount(current) < 2 ) {
        LOG(logERROR) << "XCoarsener::pre_exec: current layer contains less than 2 nodes";
        return false;
    }
    if( !m_sel->rankNodes(current) ) {
        LOG(logERROR) << "XCoarsener::pre_exec: selector rank node failed";
        return false;
    }
    Layer top = m_dao->addLayerOnTop();
    if( top.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "XCoarsener::pre_exec: cannot add layer on top";
        return false;
    }
    return true;
}

bool XCoarsener::exec()
{
    Layer top = m_dao->topLayer();

    while( m_sel->hasNext() ) {
        // Get best node to coarsen
        SuperNode root = m_sel->next();
#ifdef MLD_SAFE
        if( root.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::exec: selectBestNode failed";
            return false;
        }
#endif
        // Select valid neighbors (not flagged)
        ObjectsPtr neighbors = m_sel->unflaggedNeighbors(root.id());
        // Merge node and edges
        root = m_merger->merge(root, neighbors);
#ifdef MLD_SAFE
        if( root.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::exec: merge failed";
            return false;
        }
#endif
        // Create top node
        SuperNode rootTop = m_dao->addNodeToLayer(top);
#ifdef MLD_SAFE
        if( rootTop.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::exec: addNodeToLayer failed";
            return false;
        }
#endif
        // Link 2 root nodes
        VLink l = m_dao->addVLink(root, rootTop);
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::exec: addVLink failed";
            return false;
        }
#endif
        // Create hlinks on top layer between root node and other top root nodes
        createHLinksTopLayer(root, rootTop);
        // Flag node as unavailable, update unflagged neighbors score
        bool ok = m_sel->flagNode(root);
#ifdef MLD_SAFE
        if( !ok ) {
            LOG(logERROR) << "XCoarsener::exec: flagNode failed";
            return false;
        }
#endif
    }
    return true;
}

bool XCoarsener::postExec()
{
    return true;
}

bool XCoarsener::createHLinksTopLayer( const SuperNode& root, const SuperNode& rootTop )
{
    ObjectsPtr hop1(m_dao->graph()->Neighbors(root.id(), m_dao->superNodeType(), Any));
    ObjectsPtr hop1Unflagged(m_sel->unflaggedNodes(hop1));
    ObjectsPtr hop1flagged(m_sel->flaggedNodes(hop1));
    ObjectsPtr nodeSet(m_dao->graph()->Neighbors(hop1Unflagged.get(), m_dao->superNodeType(), Any));

    // Keep only unflagged 2-hop's flagged nodes and 1hop flagged nodes
    nodeSet = m_sel->flaggedNodes(nodeSet);
    nodeSet->Union(hop1flagged.get());

    // Add root node to the set of unflag node to retrieve the edge between
    // 1 hop flagged nodes and the root node
    hop1Unflagged->Add(root.id());

    // For each flagged node, get root node, get Vlink and
    // hlink rootTop and 2hopTop node
    ObjectsIt it(nodeSet->Iterator());
    while( it->HasNext() ) {
        auto current = it->Next();
        ObjectsPtr currentNeighbors(m_dao->graph()->Neighbors(current, m_dao->superNodeType(), Any));
        // Get root node for current node
        ObjectsPtr rootSet(Objects::CombineIntersection(currentNeighbors.get(), m_sel->rootNodes()));
        // Remove root id from the root set, we seek the 2 or 3 hop root nodes
        rootSet->Remove(root.id());

        if( rootSet->Count() != 1 ) {
            LOG(logERROR) << " XCoarsener::createHLinksTopLayer: current node as " << rootSet->Count() << " nodes.";
            return false;
        }
        // Get any root node
        auto curRoot = rootSet->Any();
        auto parents = m_dao->getParentNodes(curRoot);
        if( parents.empty() ) {
            LOG(logERROR) << "XSelector::flagNode: current node's root has no parents";
            return false;
        }
        SuperNode currentRootTop = parents.at(0);
        updateOrCreateHLink(current, currentNeighbors, hop1Unflagged, rootTop, currentRootTop);
    }
    return true;
}

bool XCoarsener::updateOrCreateHLink( oid_t currentNode,
                                     ObjectsPtr& currentNeighbors,
                                     ObjectsPtr& rootNeighbors,
                                     const SuperNode& rootTop,
                                     const SuperNode& currentRootTop )
{
    // Retrieve edges between root neighbors and 2-hop current neighbors, at least 1 element
    ObjectsPtr targets(Objects::CombineIntersection(currentNeighbors.get(), rootNeighbors.get()));
#ifdef MLD_SAFE
    if( targets->Count() == 0 ) {
        LOG(logERROR) << " XSelector::updateOrCreateHLink: cannot retrieve 1-hop node from 2hop neighbors";
        return false;
    }
#endif
    double totalWeight = totalEdgeWeight(currentNode, targets, m_dao.get());
    // Create or update final HLink in top layer between 2 top root supernodes
    HLink topLink = m_dao->getHLink(rootTop.id(), currentRootTop.id());
    if( topLink.id() == Objects::InvalidOID ) { // Create HLink
        HLink res = m_dao->addHLink(rootTop.id(), currentRootTop.id(), totalWeight);
#ifdef MLD_SAFE
        if( res.id() == Objects::InvalidOID ) {
            LOG(logERROR) << " XSelector::updateOrCreateHLink: Failed to HLink top root nodes";
            return false;
        }
#endif
    }
    else { // Update existing HLink
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
