/****************************************************************************
**
** Copyright (C) 2014 EPFL-LTS2
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

#include <boost/progress.hpp>

#include "mld/operator/selector/NeighborSelector.h"
#include "mld/operator/coarsener/NeighborCoarsener.h"
#include "mld/operator/merger/NeighborMerger.h"
#include "mld/dao/MLGDao.h"
#include "mld/utils/Timer.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;


NeighborCoarsener::NeighborCoarsener( Graph* g )
    : AbstractCoarsener(g)
{
}

NeighborCoarsener::~NeighborCoarsener()
{
}

std::string NeighborCoarsener::name() const
{
    std::string name("NeighborCoarsener: ");
    name += std::to_string(m_reductionFac * 100) + "% ";
    if( m_sel && m_merger )
        name += m_sel->name() + " " + m_merger->name();
    return name;
}

void NeighborCoarsener::setSelector( NeighborSelector* selector )
{
    m_sel.reset(selector);
}

void NeighborCoarsener::setMerger( NeighborMerger* merger )
{
    m_merger.reset(merger);
}

bool NeighborCoarsener::preExec()
{
    if( !m_sel || !m_merger ) {
        LOG(logERROR) << "NeighborCoarsener::preExec: No selector and/or merger set, please set them first";
        return false;
    }

    std::unique_ptr<Timer> t(new Timer("NeighborCoarsener::preExec"));
    // Check current layer node count
    Layer current(m_dao->topLayer());
    if( m_dao->getNodeCount(current) < 2 ) {
        LOG(logERROR) << "NeighborCoarsener::preExec: current layer contains less than 2 nodes";
        return false;
    }

    Layer top = m_dao->mirrorTopLayer();
    if( top.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "NeighborCoarsener::preExec: mirroring layer failed";
        return false;
    }

    return true;
}

bool NeighborCoarsener::exec()
{
    std::unique_ptr<Timer> t(new Timer("NeighborCoarsener::exec"));
    Layer base(m_dao->baseLayer());
    Layer current(m_dao->topLayer());

    int64_t numVertices = m_dao->getNodeCount(base);
    int64_t mergeCount = computeMergeCount(numVertices);

    if( !m_sel->rankNodes(current) ) { // Rank nodes
        LOG(logERROR) << "NeighborCoarsener::exec: selector rank nodes failed";
        return false;
    }

//    auto beforeMerge = mergeCount;
    LOG(logINFO) << "Start coarsening, " << mergeCount << " nodes to merge";
    ProgressDisplay display(mergeCount);

    while( mergeCount > 0 ) {
        // Get best node to coarsen
        SuperNode root(m_sel->next());
#ifdef MLD_SAFE
        if( root.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "NeighborCoarsener::exec: selectBestNode failed";
            return false;
        }
#endif
//        LOG(logDEBUG) << "Root: " << root;
        // Still node to collpase but selector has spent all his node
        if( !m_sel->hasNext() ) {
            LOG(logINFO) << "Selector is exhausted, rerank";
            if( !m_sel->rankNodes(current) ) { // Rank nodes
                LOG(logERROR) << "NeighborCoarsener::exec: 2nd selector rank node failed";
                return false;
            }
        }

        ObjectsPtr neighbors(m_sel->getNodesToMerge());
        if( !neighbors ) {
            return false;
        }

        auto neighborsCount = neighbors->Count();
        // Merge node and edges, re-route VLinks
        if( !m_merger->merge(root, neighbors) ) {
            LOG(logERROR) << "NeighborCoarsener::exec: Merger failed to collapse root and neighbors " << root;
            return false;
        }
        // Decrease mergeCount number
        auto count = std::max(int64_t(1), neighborsCount);
        mergeCount -= count;

        display += count;
    }

    return true;
}

bool NeighborCoarsener::postExec()
{
    return true;
}

