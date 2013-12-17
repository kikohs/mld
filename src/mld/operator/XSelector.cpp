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

#include "mld/Graph_types.h"
#include "mld/operator/XSelector.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace dex::gdb;

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

bool XSelector::flagNode( const SuperNode& node )
{
    // Update 2 hop neighbors
    // Flag node + neighbors
    return false;
}

bool XSelector::isFlagged( oid_t snid )
{
    return m_flagged->Exists(snid);
}

float XSelector::calcScore( oid_t snid )
{
    // TODO
    return 1.0;
}
