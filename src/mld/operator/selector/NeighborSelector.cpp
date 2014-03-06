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

#include "mld/operator/selector/NeighborSelector.h"
#include "mld/GraphTypes.h"
#include "mld/dao/MLGDao.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;

NeighborSelector::NeighborSelector( Graph* g )
    : AbstractSelector(g)
    , m_hasMemory(false)
    , m_layerId(Objects::InvalidOID)
    , m_root(Objects::InvalidOID)
    , m_flagged( m_dao->newObjectsPtr() )
    , m_curNeighbors( m_dao->newObjectsPtr() )
    , m_nodesToUpdate( m_dao->newObjectsPtr() )
{
}

NeighborSelector::~NeighborSelector()
{
}

void NeighborSelector::resetSelection()
{
    m_layerId = Objects::InvalidOID;
    m_root = Objects::InvalidOID;
    m_flagged->Clear();
    m_curNeighbors->Clear();
    m_nodesToUpdate->Clear();
    m_scores.clear();
}

bool NeighborSelector::rankNodes( const Layer& layer )
{    
    resetSelection();
    m_layerId = layer.id();
    ObjectsPtr nodes(m_dao->getAllNodeIds(layer));

    LOG(logINFO) << "Ranking nodes";
    ProgressDisplay display(nodes->Count());

    // Iterate through each node and calculate score
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() ) {
        oid_t nid = it->Next();
        if( nid == Objects::InvalidOID ) {
            LOG(logERROR) << "NeighborSelector::rankNodes invalid node";
            return false;
        }
        // Add values in the mutable priority queue, no nodes are flagged
        m_scores.push(nid, calcScore(nid));
        ++display;
    }
    // Set first value
    m_root = m_scores.front_value();
    return true;
}

bool NeighborSelector::hasNext()
{
    return !m_scores.empty();
}

SuperNode NeighborSelector::next()
{
    if( m_scores.empty() ) {
        LOG(logWARNING) << "NeighborSelector::next empty scores";
        resetSelection();
        return SuperNode();
    }

    // Update score form each node marked as to be updated
    // in method setNodesToUpdate
    if( !updateScores() ) {
        resetSelection();
        LOG(logERROR) << "NeighborSelector::next updateScores failed";
        return SuperNode();
    }

    if( m_hasMemory )
        m_flagged->Add(m_root);

    // Get a new best candidate
    m_root = m_scores.front_value();
    // Set the best neighbors for the merger, use m_root
    // to set m_curNeighors in child class
    setNodesToMerge();
    // Cherry pick nodes to update in the next iteration
    setNodesToUpdate();
    // Clear previous node neighborhood from selection queue
    delNodesToMergeFromQueue();
    return m_dao->getNode(m_root);
}

void NeighborSelector::delNodesToMergeFromQueue()
{
    ObjectsIt it(m_curNeighbors->Iterator());
    while( it->HasNext() ) {
        m_scores.erase(it->Next());
    }

    if( m_hasMemory )
        m_scores.erase(m_root);
}

ObjectsPtr NeighborSelector::getNeighbors( oid_t snid )
{
#ifdef MLD_SAFE
    if( snid == Objects::InvalidOID ) {
        return ObjectsPtr();
    }
#endif

    ObjectsPtr neighbors(m_dao->graph()->Neighbors(snid, m_dao->hlinkType(), Any));
    if( m_hasMemory ) {
        // Return only neighbors not flagged
        neighbors->Difference(m_flagged.get());
    }
    return neighbors;
}

ObjectsPtr NeighborSelector::getNeighbors( const ObjectsPtr& input )
{
#ifdef MLD_SAFE
    if( !input )
        return ObjectsPtr();
#endif

    ObjectsPtr neighbors(m_dao->graph()->Neighbors(input.get(), m_dao->hlinkType(), Any));
    if( m_hasMemory ) {
        // Return only neighbors not flagged
        neighbors->Difference(m_flagged.get());
    }
    return neighbors;
}

ObjectsPtr NeighborSelector::getNodesToMerge()
{
    return ObjectsPtr(m_curNeighbors->Copy());
}
