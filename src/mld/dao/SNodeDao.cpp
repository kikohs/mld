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
#include <dex/gdb/Graph_data.h>
#include <dex/gdb/Value.h>

#include "mld/Graph_types.h"
#include "mld/dao/SNodeDao.h"

using namespace mld;
using namespace dex::gdb;

SNodeDao::SNodeDao( dex::gdb::Graph* g )
    : AbstractDao(g)
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

void SNodeDao::removeNode( dex::gdb::oid_t id )
{
#ifdef MLD_SAFE
    if( id == Objects::InvalidOID )
        return;
#endif
#ifdef MLD_SAFE
    try {
#endif
        m_g->Drop(id);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "SNodeDao::removeNode " << e.Message();
    }
#endif
}

void SNodeDao::updateNode( const SuperNode& n )
{
#ifdef MLD_SAFE
    if( n.id() == Objects::InvalidOID )
        return;
#endif
    attr_t att = m_g->FindAttribute(m_snType, SNAttr::WEIGHT);
    m_v->SetDoubleVoid(n.weight());
    m_g->SetAttribute(n.id(), att, *m_v);
}

SuperNode SNodeDao::getNode( dex::gdb::oid_t id )
{
#ifdef MLD_SAFE
    if( id == Objects::InvalidOID )
        return SuperNode();
#endif

#ifdef MLD_SAFE
    try {
#endif
        m_g->GetAttribute(id, m_g->FindAttribute(m_snType, SNAttr::WEIGHT), *m_v);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "SNodeDao::getNode: " << e.Message();
        return SuperNode();
    }
#endif
    return SuperNode(id, m_v->GetDouble());
}


std::vector<SuperNode> SNodeDao::getNode( const ObjectsPtr& objs )
{
    std::vector<SuperNode> res;
    res.reserve(objs->Count());
    ObjectsIt it(objs->Iterator());
    while( it->HasNext() ) {
        SuperNode n = getNode(it->Next());
#ifdef MLD_SAFE
        // Watch for the if, no { }
        if( n.id() != dex::gdb::Objects::InvalidOID )
#endif
            res.push_back(n);
    }
    return res;
}





