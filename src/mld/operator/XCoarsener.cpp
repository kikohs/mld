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

#include "mld/dao/MLGDao.h"
#include "mld/operator/XCoarsener.h"
#include "mld/operator/XSelector.h"
#include "mld/operator/MultiAdditiveMerger.h"
#include "mld/utils/Timer.h"

using namespace mld;
using namespace sparksee::gdb;

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
    Layer current(m_dao->topLayer());
    if( m_dao->getNodeCount(current) < 2 ) {
        LOG(logERROR) << "XCoarsener::pre_exec: current layer contains less than 2 nodes";
        return false;
    }
    Layer top(m_dao->addLayerOnTop());
    if( top.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "XCoarsener::pre_exec: cannot add layer on top";
        return false;
    }
    return true;
}

bool XCoarsener::exec()
{
    std::unique_ptr<Timer> t(new Timer("XCoarsener::exec"));
    Layer top(m_dao->topLayer());
    Layer prev(m_dao->child(top));

    int64_t numVertices = m_dao->getNodeCount(prev);
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
    if( !m_sel->rankNodes(prev) ) { // Rank nodes
        LOG(logERROR) << "XCoarsener::firstPass: selector rank node failed";
        return false;
    }

    // While there are nodes to collapse and all nodes are not flagged
    while( m_sel->hasNext() && mergeCount > 0 ) {
        // Get best node to coarsen
        SuperNode root(m_sel->next());
        // Select valid neighbors (not flagged)
        ObjectsPtr neighbors(m_sel->getUnflaggedNeighbors(root.id()));

        // Create top node
        SuperNode rootTop(m_dao->addNodeToLayer(top));
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
        VLink l(m_dao->addVLink(root, rootTop));

        // Create VLinks from neighbors to rootTop SuperNode
        ObjectsIt it(neighbors->Iterator());
        while( it->HasNext() ) {
            auto id = it->Next();
            m_dao->addVLink(SuperNode(id), rootTop);
        }
        // Create hlinks on top layer between root node and other top root nodes
        if( !createTopHLinks1N2Hops(root, rootTop) ) {
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
        mergeCount -= std::max(int64_t(1), neighbors->Count());
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
        SuperNode root(m_dao->getNode(it->Next()));
        LOG(logDEBUG) << "XCoarsener::mirrorRemainingNodes root: " << root;
        // Create top node
        SuperNode rootTop(m_dao->addNodeToLayer(top));
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

    if( !m_sel->rankNodes(top) ) { // Rank nodes
        LOG(logERROR) << "XCoarsener::secondPass: selector rank node failed";
        return false;
    }

    // While there are node to collapse
    while( m_sel->hasNext() && mergeCount > 0 ) {
        // Get best node to coarsen
        SuperNode root(m_sel->next());
#ifdef MLD_SAFE
        if( root.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::secondPass: selectBestNode failed";
            return false;
        }
#endif
        // Select valid neighbors (not flagged)
        ObjectsPtr neighbors(m_sel->getUnflaggedNeighbors(root.id()));
        // Merge node and edges, re-route VLinks
        if( !m_merger->merge(root, neighbors) ) {
            LOG(logERROR) << "XCoarsener::secondPass: Merger failed to collapse root and neighbors " << root;
            return false;
        }
        // Decrease mergeCount number
        mergeCount -= std::max(int64_t(1), neighbors->Count());
    }
    return true;
}

bool XCoarsener::createTopHLinks1Hop( const SuperNode& root, const SuperNode& rootTop )
{
    ObjectsPtr hop1(m_dao->graph()->Neighbors(root.id(), m_dao->hlinkType(), Any));
    ObjectsPtr nodeSet(m_sel->getFlaggedNodesFrom(hop1));
    return createTopHLinks(root, rootTop, nodeSet);
}

bool XCoarsener::createTopHLinks1N2Hops( const SuperNode& root, const SuperNode& rootTop )
{
    ObjectsPtr hop1(m_dao->graph()->Neighbors(root.id(), m_dao->hlinkType(), Any));
    ObjectsPtr hop1flagged(m_sel->getFlaggedNodesFrom(hop1));
    ObjectsPtr hop1Unflagged(m_sel->getUnflaggedNodesFrom(hop1));
    ObjectsPtr nodeSet(m_dao->graph()->Neighbors(hop1Unflagged.get(), m_dao->hlinkType(), Any));

    // Keep only unflagged 2-hop's flagged nodes and 1hop flagged nodes
    nodeSet = m_sel->getFlaggedNodesFrom(nodeSet);
    nodeSet->Union(hop1flagged.get());

    // For each flagged node, get root node, get Vlink and
    // hlink rootTop and 2hopTop node
    return createTopHLinks(root, rootTop, nodeSet);
}

bool XCoarsener::createTopHLinks( const SuperNode& root, const SuperNode& rootTop, const ObjectsPtr& nodeSet )
{
    ObjectsIt it(nodeSet->Iterator());
    while( it->HasNext() ) {
        auto current = it->Next();
        auto parents = m_dao->getParentNodes(current);
        if( parents.empty() ) {
            LOG(logERROR) << " XCoarsener::createTopHLinks: flagged node as no parents";
            return false;
        }
        SuperNode top = parents.at(0);
        HLink currentLink = m_dao->getHLink(root.id(), current);
        HLink topLink = m_dao->getHLink(rootTop.id(), top.id());
        if( topLink.id() == Objects::InvalidOID ) {  // Top HLink doesn't exist
            m_dao->addHLink(rootTop.id(), top.id(), currentLink.weight());
        }
        else {  // Update top HLink
            topLink.setWeight( topLink.weight() + currentLink.weight() );
            if( !m_dao->updateHLink(topLink) ) {
                LOG(logERROR) << " XCoarsener::createTopHLinks: Failed to update top root nodes HLINK";
                return false;
            }
        }
    }
    return true;
}
