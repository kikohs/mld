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
#include "mld/utils/Timer.h"

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
            LOG(logERROR) << " XCoarsener::totalEdgeWeight: Invalid retrieved HLink";
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
    : AbstractCoarsener(g)
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
    Layer top = m_dao->addLayerOnTop();
    if( top.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "XCoarsener::pre_exec: cannot add layer on top";
        return false;
    }
    return true;
}

bool XCoarsener::exec()
{
    std::unique_ptr<Timer> t(new Timer("XCoarsener::exec"));
    Layer top = m_dao->topLayer();
    Layer prev = m_dao->child(top);

    auto numVertices = m_dao->getNodeCount(prev);
    int64_t mergeCount = computeMergeCount(numVertices);

    // Coarsen prev layer and create top root nodes until mergeCount
    // is reached or selector has flagged every node in the layer
    if( !firstPass(prev, top, mergeCount) ) {
        LOG(logERROR) << "XCoarsener::exec: first pass failed";
        return false;
    }

    auto nodes = m_sel->remainingNodes();
    if( nodes->Count() != 0 ) {  // If coarsening stopped before all nodes have been coarsened
        if( !mirrorRemainingNodes(top, nodes) ) {
            LOG(logERROR) << "XCoarsener::exec: mirroring failed";
            return false;
        }
    }
    else {  // Selector has flagged all nodes
        if( mergeCount > 0 ) {  // Top layer has still too many nodes
            if( !secondPass(top, mergeCount) ) {  // coarsen top layer
                LOG(logERROR) << "XCoarsener::exec: second pass failed";
                return false;
            }
        }
    }
    return true;
}

bool XCoarsener::postExec()
{
    return true;
}

bool XCoarsener::firstPass( const Layer& prev, const Layer& top, int64_t& mergeCount )
{
    std::unique_ptr<Timer> t(new Timer("XCoarsener::firstPass"));
    LOG(logDEBUG) << "First pass rank start";
    if( !m_sel->rankNodes(prev) ) { // Rank nodes
        LOG(logERROR) << "XCoarsener::firstPass: selector rank node failed";
        return false;
    }

    // While there are nodes to collapse and all nodes are not flagged
    while( m_sel->hasNext() && mergeCount > 0 ) {
        // Get best node to coarsen
        SuperNode root = m_sel->next();
        // Select valid neighbors (not flagged)
        ObjectsPtr neighbors = m_sel->getUnflaggedNeighbors(root.id());

        // Create top node
        SuperNode rootTop = m_dao->addNodeToLayer(top);
#ifdef MLD_SAFE
        if( rootTop.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::firstPass: addNodeToLayer failed";
            return false;
        }
#endif
        // Update rootTop weight with the merged root weight on previous layer
        rootTop.setWeight(m_merger->computeWeight(root, neighbors));
        m_dao->updateNode(rootTop);

        // Link 2 root nodes
        VLink l = m_dao->addVLink(root, rootTop);
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::firstPass: addVLink failed";
            return false;
        }
#endif
        // Create hlinks on top layer between root node and other top root nodes
        if( !createTopHLinks2Hops(root, rootTop) ) {
            LOG(logERROR) << "XCoarsener::firstPass: createTopHLinks2Hops failed";
            return false;
        }

        // Set as root node for this layer
        root.setRoot(true);
        m_dao->updateNode(root);
        // Flag node as unavailable, update unflagged neighbors score
        if( !m_sel->flagAndUpdateScore(root) ) {
            LOG(logERROR) << "XCoarsener::firstPass: flagNode failed";
            return false;
        }

        // Decrease mergeCount number
        if( !neighbors ) {
            mergeCount--;
        } else {
            mergeCount -= std::max(int64_t(1), neighbors->Count());
        }
    }
    return true;
}

bool XCoarsener::mirrorRemainingNodes( const Layer& top, const ObjectsPtr& nodes )
{
    LOG(logDEBUG) << "XCoarsener::mirrorRemainingNodes";
    std::unique_ptr<Timer> t(new Timer("XCoarsener::mirrorRemainingNodes"));
    // No all node have been coarsened, we need to duplicate them on the top layer
    if( nodes->Count() == 0 ) {
        LOG(logWARNING) << "XCoarsener::mirrorRemainingNodes: empty node set";
        return true;
    }

    // Create VLINKS
    ObjectsIt it(nodes->Iterator());
    // For each node
    while( it->HasNext() ) {
        SuperNode root = it->Next();
        LOG(logDEBUG) << "XCoarsener::mirrorRemainingNodes root: " << root;
        // Create top node
        SuperNode rootTop = m_dao->addNodeToLayer(top);
        // Update rootTop weight
        rootTop.setWeight(root.weight());
        m_dao->updateNode(rootTop);
        // Link 2 root nodes
        m_dao->addVLink(root, rootTop);

        if( !createTopHLinks1Hop(root, rootTop) ) {
            LOG(logERROR) << "XCoarsener::mirrorRemainingNodes: failed to create top HLink";
            return false;
        }

        // Set as root node for this layer
        root.setRoot(true);
        m_dao->updateNode(root);
        // Add node in flagged set
        m_sel->rootNodes()->Add(root.id());
        m_sel->flaggedNodes()->Add(root.id());
    }
    return true;
}

bool XCoarsener::secondPass( const Layer& top, int64_t& mergeCount )
{
    std::unique_ptr<Timer> t(new Timer("XCoarsener::secondPass"));
    LOG(logDEBUG) << "XCoarsener::secondPass";

    if( !m_sel->rankNodes(top) ) { // Rank nodes
        LOG(logERROR) << "XCoarsener::secondPass: selector rank node failed";
        return false;
    }

    // While there are node to collapse
    while( mergeCount > 0 ) {
        // Get best node to coarsen
        SuperNode root = m_sel->next();
#ifdef MLD_SAFE
        if( root.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::secondPass: selectBestNode failed";
            return false;
        }
#endif
        // Select valid neighbors (not flagged)
        ObjectsPtr neighbors = m_sel->getUnflaggedNeighbors(root.id());
        // Merge node and edges, re-route VLinks
        if( !m_merger->merge(root, neighbors) ) {
            LOG(logERROR) << "XCoarsener::secondPass: Merger failed to collapse root and neighbors " << root;
            return false;
        }
        // Decrease mergeCount number
        mergeCount -= neighbors->Count();
    }
    return true;
}

bool XCoarsener::createTopHLinks1Hop( const SuperNode& root, const SuperNode& rootTop )
{
    LOG(logDEBUG) << "XCoarsener::createTopHLinks1Hop root:" << root;
    ObjectsPtr hop1(m_dao->graph()->Neighbors(root.id(), m_dao->hlinkType(), Any));
    ObjectsPtr hop1flagged(m_sel->getFlaggedNodesFrom(hop1));

    // Print root nodes
//    LOG(logDEBUG) << "Root nodes";
//    ObjectsIt it3(m_sel->rootNodes()->Iterator());
//    while( it3->HasNext() )
//        LOG(logDEBUG) << it3->Next();

//    LOG(logDEBUG) << "";

    ObjectsIt it(hop1flagged->Iterator());
    while( it->HasNext() ) {
        auto current = it->Next();
        ObjectsPtr currentNeighbors(m_dao->graph()->Neighbors(current, m_dao->hlinkType(), Any));
        // Get root node for current node, add current node in set because it can be a root node,
        // now that we are mirroring nodes, a direct neighbors can be a root node
        currentNeighbors->Add(current);
        ObjectsPtr rootSet(Objects::CombineIntersection(currentNeighbors.get(), m_sel->rootNodes()));
        if( rootSet->Count() == 0 ) {
            LOG(logERROR) << "XCoarsener::createTopHLinks1Hop:" \
                             "a flagged node should have a root node in its neighbors";
            return false;
        }
        // FIXME change to create link to all root nodes an edge exists between root and current
        auto curRoot = rootSet->Any();
        auto parents = m_dao->getParentNodes(curRoot);
        if( parents.empty() ) {
            LOG(logERROR) << "XCoarsener::flagNode: current node's root has no parents";
            return false;
        }
        // Only 1 parent
        SuperNode currentRootTop = parents.at(0);

        // Retrieve existing HLink between root and 1hop direct neighbor
        HLink l = m_dao->getHLink(current, root.id());
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << " XCoarsener::createTopHLinks1Hop: Invalid retrieved HLink";
            return false;
        }
#endif
        // Create HLink from rootTop and and 1hop neighbors's top root node
        l = m_dao->addHLink(rootTop, currentRootTop, l.weight());
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << " XCoarsener::createTopHLinks1Hop: Create HLink failed";
            return false;
        }
#endif
    }
    return true;
}

bool XCoarsener::createTopHLinks2Hops( const SuperNode& root, const SuperNode& rootTop )
{
    LOG(logDEBUG) << "XCoarsener::createTopHLinks2Hops";
    ObjectsPtr hop1(m_dao->graph()->Neighbors(root.id(), m_dao->hlinkType(), Any));
    ObjectsPtr hop1Unflagged(m_sel->getUnflaggedNodesFrom(hop1));
    ObjectsPtr hop1flagged(m_sel->getFlaggedNodesFrom(hop1));
    ObjectsPtr nodeSet(m_dao->graph()->Neighbors(hop1Unflagged.get(), m_dao->hlinkType(), Any));

    // Keep only unflagged 2-hop's flagged nodes and 1hop flagged nodes
    nodeSet = m_sel->getFlaggedNodesFrom(nodeSet);
    nodeSet->Union(hop1flagged.get());

    // Add root node to the set of unflag node to retrieve the edge between
    // 1 hop flagged nodes and the root node
    hop1Unflagged->Add(root.id());

    // For each flagged node, get root node, get Vlink and
    // hlink rootTop and 2hopTop node
    ObjectsIt it(nodeSet->Iterator());
    while( it->HasNext() ) {
        auto current = it->Next();
        ObjectsPtr currentNeighbors(m_dao->graph()->Neighbors(current, m_dao->hlinkType(), Any));
        // Get root node for current node
        ObjectsPtr rootSet(Objects::CombineIntersection(currentNeighbors.get(), m_sel->rootNodes()));
        // Remove root id from the root set, we seek the 2 or 3 hop root nodes
        rootSet->Remove(root.id());
        if( rootSet->Count() == 0 ) {
            LOG(logERROR) << "XCoarsener::createTopHLinks2Hop:" \
                             "a flagged node should have a root node in its neighbors";
            return false;
        }
        // Get any root node
        auto curRoot = rootSet->Any();
        auto parents = m_dao->getParentNodes(curRoot);
        if( parents.empty() ) {
            LOG(logERROR) << "XCoarsener::createTopHLinks2Hops: current node's root has no parents";
            return false;
        }
        // Only 1 parent
        SuperNode currentRootTop = parents.at(0);
        if( !updateOrCreateHLink(current, currentNeighbors,
                                  hop1Unflagged, rootTop, currentRootTop) ) {
            LOG(logERROR) << "XCoarsener::createTopHLinks2Hops: updateOrCreateHLink failed";
            return false;
        }

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
        LOG(logERROR) << " XCoarsener::updateOrCreateHLink: cannot retrieve 1-hop node from 2hop neighbors";
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
            LOG(logERROR) << " XCoarsener::updateOrCreateHLink: Failed to HLink top root nodes";
            return false;
        }
#endif
    }
    else {  // Update existing HLink
        topLink.setWeight(topLink.weight() + totalWeight);
        // Update weight
        bool ok = m_dao->updateHLink(topLink);
#ifdef MLD_SAFE
        if( !ok ) {
            LOG(logERROR) << " XCoarsener::updateOrCreateHLink: Failed to update top root nodes HLINK";
            return false;
        }
#endif
    }
    return true;
}
