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
    std::unique_ptr<Timer> t(new Timer("XCoarsener::preExec"));
    // Check current layer node count
    Layer current(m_dao->topLayer());
    if( m_dao->getNodeCount(current) < 2 ) {
        LOG(logERROR) << "XCoarsener::preExec: current layer contains less than 2 nodes";
        return false;
    }

    if( !m_singlePass ) {  // Single pass will mirror the top layer
        Layer top(m_dao->addLayerOnTop());
        if( top.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::preExec: cannot add layer on top";
            return false;
        }
    } else {
        Layer top = m_dao->mirrorTopLayer();
        if( top.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::preExec: mirroring layer failed";
            return false;
        }
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

    if( m_singlePass )
        return singlePass(top, mergeCount);

    return multiPass(prev, top, mergeCount);
}

bool XCoarsener::postExec()
{
    return true;
}

bool XCoarsener::singlePass( const Layer& top, int64_t& mergeCount )
{
    if( mergeCount > 0 ) {
        if( !topLayerPass(top, mergeCount) ) {  // coarsen top layer
            LOG(logERROR) << "XCoarsener::singlePass: topLayerPass failed";
            return false;
        }
    } else {
        LOG(logERROR) << "XCoarsener::singlePass: mergeCount < 0";
        return false;
    }
    return true;
}

bool XCoarsener::multiPass( const Layer& prev, const Layer& top, int64_t& mergeCount )
{
    // Coarsen prev layer and create top root nodes until mergeCount
    // is reached or selector has flagged every node in the layer
    if( !currentLayerPass(prev, top, mergeCount) ) {
        LOG(logERROR) << "XCoarsener::multiPass: first pass failed";
        return false;
    }

    // If coarsening stopped before all nodes have been coarsened
    if( m_sel->hasNext() ) {
        if( !mirrorRemainingNodes(top) ) {
            LOG(logERROR) << "XCoarsener::multiPass: mirroring failed";
            return false;
        }
    }
    else {  // Selector has flagged all nodes
        if( mergeCount > 0 ) {  // Top layer has still too many nodes
            if( !topLayerPass(top, mergeCount) ) {  // coarsen top layer
                LOG(logERROR) << "XCoarsener::multiPass: second pass failed";
                return false;
            }
        }
    }
    return true;
}

bool XCoarsener::currentLayerPass( const Layer& prev, const Layer& top, int64_t& mergeCount )
{
    std::unique_ptr<Timer> t(new Timer("XCoarsener::currentLayerPass"));
    if( !m_sel->rankNodes(prev) ) { // Rank nodes
        LOG(logERROR) << "XCoarsener::currentLayerPass: selector rank node failed";
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
            LOG(logERROR) << "XCoarsener::currentLayerPass: addNodeToLayer failed";
            return false;
        }
#endif
        // Update rootTop weight with the merged root weight on previous layer
        rootTop.setWeight(m_merger->computeWeight(root, neighbors));
        m_dao->updateNode(rootTop);

        // Link 2 root nodes
        m_dao->addVLink(root, rootTop);

        // Create VLinks from neighbors to rootTop SuperNode
        ObjectsIt it(neighbors->Iterator());
        while( it->HasNext() ) {
            m_dao->addVLink(SuperNode(it->Next()), rootTop);
        }
        // Create hlinks on top layer between root node and other top root nodes
        ObjectsPtr nodeSet(m_sel->getHLinkEnpoints(root, false));
        if( !m_dao->verticalCopyHLinks(root, rootTop, MLGDao::TOP, false, nodeSet) ) {
            LOG(logERROR) << "XCoarsener::currentLayerPass: verticalCopyHLinks failed";
            return false;
        }

        // Set as root node for this layer
        root.setRoot(true);
        m_dao->updateNode(root);
        // Flag node as unavailable, update unflagged neighbors score
        if( !m_sel->flagAndUpdateScore(root) ) {
            LOG(logERROR) << "XCoarsener::currentLayerPass: flagNode failed";
            return false;
        }

        // Decrease mergeCount number
        mergeCount -= std::max(int64_t(1), neighbors->Count());
    }
    return true;
}

bool XCoarsener::mirrorRemainingNodes( const Layer& top )
{
    std::unique_ptr<Timer> t(new Timer("XCoarsener::mirrorRemainingNodes"));
    // No all node have been coarsened, we need to duplicate them on the top layer
    // Create VLINKS for each node
    while( m_sel->hasNext() ) {
        SuperNode root(m_sel->next());
//        LOG(logDEBUG) << "XCoarsener::mirrorRemainingNodes root: " << root;
        // Create top node
        SuperNode rootTop(m_dao->addNodeToLayer(top));
        // Update rootTop weight
        rootTop.setWeight(root.weight());
        m_dao->updateNode(rootTop);
        // Link 2 root nodes
        m_dao->addVLink(root, rootTop);

        ObjectsPtr nodeSet(m_sel->getHLinkEnpoints(root, true));
        if( !m_dao->verticalCopyHLinks(root, rootTop, MLGDao::TOP, false, nodeSet) ) {
            LOG(logERROR) << "XCoarsener::mirrorRemainingNodes: failed to create top HLink";
            return false;
        }

        // Set as root node for this layer
        root.setRoot(true);
        m_dao->updateNode(root);
        // Flag node as root but do not remove neighbors
        if( !m_sel->flagAndUpdateScore(root, false) ) {
            LOG(logERROR) << "XCoarsener::mirrorRemainingNodes: flagNode failed";
            return false;
        }
    }
    return true;
}

bool XCoarsener::topLayerPass( const Layer& top, int64_t& mergeCount )
{
    std::unique_ptr<Timer> t(new Timer("XCoarsener::topLayerPass"));

    if( !m_sel->rankNodes(top) ) { // Rank nodes
        LOG(logERROR) << "XCoarsener::topLayerPass: selector rank node failed";
        return false;
    }

    while( mergeCount > 0 ) {
        // Get best node to coarsen
        SuperNode root;
        if( m_singlePass )
            root = m_sel->next(false);  // do not remove node from queue
        else
            root = m_sel->next(true);

#ifdef MLD_SAFE
        if( root.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::topLayerPass: selectBestNode failed";
            return false;
        }
#endif

        // Still node to collpase but selector has spent all his node
        if( !m_singlePass && !m_sel->hasNext() ) {
            if( !m_sel->rankNodes(top) ) { // Rank nodes
                LOG(logERROR) << "XCoarsener::topLayerPass: 2nd selector rank node failed";
                return false;
            }
        }

        ObjectsPtr neighbors;
        if( !m_singlePass ) {  // Update neighbors scores and flagged nodes
            neighbors = m_sel->getUnflaggedNeighbors(root.id());
            m_sel->flagAndUpdateScore(root, true, false);

        }
        else {  // Single pass, no flagged nodes, select all neighbors
            neighbors.reset(m_dao->graph()->Neighbors(root.id(), m_dao->hlinkType(), Any));
        }
        auto neighborsCount = neighbors->Count();

        // Merge node and edges, re-route VLinks
        if( !m_merger->merge(root, neighbors) ) {
            LOG(logERROR) << "XCoarsener::topLayerPass: Merger failed to collapse root and neighbors " << root;
            return false;
        }

        if( m_singlePass ) {  // In case of single pass, we update node score by hand
            m_sel->removeCandidates(neighbors);
            // New neighbors now that nodes have been merged
            neighbors.reset(m_dao->graph()->Neighbors(root.id(), m_dao->hlinkType(), Any));
            neighbors->Add(root.id());
            if( !m_sel->updateScore(neighbors, true) ) {
                LOG(logERROR) << "XCoarsener::topLayerPass: Could not update score for single pass: " << root;
                return false;
            }
        }

        // Decrease mergeCount number
        mergeCount -= std::max(int64_t(1), neighborsCount);
    }
    return true;
}
