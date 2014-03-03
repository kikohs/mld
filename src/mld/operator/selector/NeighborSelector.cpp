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

using namespace mld;
using namespace sparksee::gdb;

NeighborSelector::NeighborSelector( Graph* g )
    : AbstractSelector(g)
    , m_hasMemory(false)
    , m_layerId(Objects::InvalidOID)
    , m_current(Objects::InvalidOID)
    , m_flagged( m_dao->newObjectsPtr() )
    , m_curNeighbors( m_dao->newObjectsPtr() )
{
}

NeighborSelector::~NeighborSelector()
{
}

bool NeighborSelector::rankNodes( const Layer& layer )
{
    // Reset variables
    m_scores.clear();
    m_current = Objects::InvalidOID;
    m_curNeighbors = m_dao->newObjectsPtr();
    m_flagged = m_dao->newObjectsPtr();
    m_layerId = layer.id();

    ObjectsPtr nodes(m_dao->getAllNodeIds(layer));
    // Iterate through each node and calculate score
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() ) {
        oid_t nid = it->Next();
#ifdef MLD_SAFE
        if( nid == Objects::InvalidOID ) {
            LOG(logERROR) << "NeighborSelector::rankNodes invalid node";
            return false;
        }
#endif
        // Add values in the mutable priority queue, no nodes are flagged
        m_scores.push(nid, calcScore(nid));
    }
    return true;
}

bool NeighborSelector::hasNext()
{
    return !m_scores.empty();
}

SuperNode NeighborSelector::next()
{
    if( m_scores.empty() ) {
        m_current =  Objects::InvalidOID;
        m_curNeighbors = m_dao->newObjectsPtr();
        return SuperNode();
    }

    // Clear previous node neighborhood
    removeCandidates(m_curNeighbors);
    ObjectsPtr nodeSet(getNeighbors(m_current));
    if( nodeSet )
        updateScore(nodeSet);

    if( m_hasMemory )
        m_flagged->Add(m_current);

    m_current = m_scores.front_value();
    // Set the best neighbors for the merger, use m_current
    // to set m_curNeighors
    setCurrentBestNeighbors();

    if( m_hasMemory )
        m_scores.pop();

    return m_dao->getNode(m_current);
}

bool NeighborSelector::updateScore( const ObjectsPtr& input )
{
    if( !input )
        return false;

    ObjectsIt it(input->Iterator());
    while( it->HasNext() ) {
        auto id = it->Next();
        m_scores.update(id, calcScore(id));
    }
    return true;
}

void NeighborSelector::removeCandidates( const ObjectsPtr& input )
{
    if( !input )
        return;
    // Remove candidates nodes from selection
    ObjectsIt it(input->Iterator());
    while( it->HasNext() ) {
        m_scores.erase(it->Next());
    }
}

ObjectsPtr NeighborSelector::getNeighbors( oid_t snid )
{
    if( snid == Objects::InvalidOID )
        return ObjectsPtr();

    ObjectsPtr neighbors(m_dao->graph()->Neighbors(snid, m_dao->hlinkType(), Any));
    if( m_hasMemory ) {
        // Return only neighbors not flagged
        neighbors->Difference(m_flagged.get());
    }
    return neighbors;
}
