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
#include <dex/gdb/Graph_data.h>
#include <dex/gdb/Value.h>

#include "mld/Graph_types.h"
#include "mld/dao/SNodeDao.h"

using namespace mld;
using namespace dex::gdb;

SNodeDao::SNodeDao( dex::gdb::Graph* g )
    : m_g(g)
    , m_v( new dex::gdb::Value )
    , m_snType(m_g->FindType(NodeType::SUPERNODE))
{
}

SNodeDao::~SNodeDao()
{
    // DO NOT DELETE GRAPH
}

SuperNode SNodeDao::addNode()
{
    return SuperNode( m_g->NewNode(m_snType) );
}

void SNodeDao::removeNode( const SuperNode& n )
{
    m_g->Drop(n.id());
}

void SNodeDao::updateNode( const SuperNode& n )
{
    attr_t att = m_g->FindAttribute(m_snType, SNAttr::WEIGHT);
    m_v->SetDoubleVoid(n.weight());
    m_g->SetAttribute(n.id(), att, *m_v);
}

SuperNode SNodeDao::getNode( dex::gdb::oid_t id )
{
    m_g->GetAttribute(id, m_g->FindAttribute(m_snType, SNAttr::WEIGHT), *m_v);
    return SuperNode(id, m_v->GetDouble() );
}
