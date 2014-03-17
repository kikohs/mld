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
#include <sparksee/gdb/Value.h>
#include <sparksee/gdb/Graph_data.h>
#include <sparksee/gdb/Objects.h>

#include "mld/dao/AbstractDao.h"

using namespace mld;
using namespace sparksee::gdb;

AbstractDao::AbstractDao( Graph* g )
    : m_g(g)
    , m_v( new Value )
{
}

AbstractDao::~AbstractDao()
{
    // DO NOT DELETE GRAPH
}

void AbstractDao::setGraph( Graph* g )
{
    m_g = g;
}

AttrMap AbstractDao::readAttrMap( oid_t id )
{
    AttrMap data;
#ifdef MLD_SAFE
    if( id == Objects::InvalidOID )
        return data;
#endif

    AttributeListPtr attrs(m_g->GetAttributes(id));
    AttributeListIt it(attrs->Iterator());
    while( it->HasNext() ) {
       attr_t attr = it->Next();
       Attribute* a = m_g->GetAttribute(attr);
       // Default construct a Value in the map and set it with
       // get attribute method
       m_g->GetAttribute(id, attr, data[a->GetName()]);
       delete a;
    }
    return data;
}

bool AbstractDao::updateAttrMap( type_t objType, oid_t id, AttrMap& data )
{
#ifdef MLD_SAFE
    if( id == Objects::InvalidOID )
        return false;
#endif
#ifdef MLD_SAFE
    try {
#endif
        for( auto& kv: data ) {
            attr_t att = m_g->FindAttribute(objType, kv.first);
            if( att != sparksee::gdb::Attribute::InvalidAttribute )
                m_g->SetAttribute(id, att, kv.second);
        }
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "AbstractDao::updateAttrMap: " << e.Message();
        return false;
    }
#endif
    return true;
}

oid_t AbstractDao::addEdge( type_t lType, oid_t src, oid_t tgt )
{
    oid_t eid = Objects::InvalidOID;
#ifdef MLD_SAFE
    if( src == tgt ) { // no self loop
        LOG(logWARNING) << "AbstractDao::addEdge: no self-loop allowed";
        return eid;
    }

    // no invalid edge
    if( src == Objects::InvalidOID || tgt == Objects::InvalidOID ) {
        LOG(logERROR) << "AbstractDao::addEdge: source or target is invalid";
        return eid;
    }
#endif
#ifdef MLD_SAFE
    try {
#endif
        eid = m_g->NewEdge(lType, src, tgt);
#ifdef MLD_SAFE
    } catch( Error& ) {
        LOG(logERROR) << "AbstractDao::addEdge: invalid src or tgt";
        return Objects::InvalidOID;
    }
#endif
    return eid;
}

oid_t AbstractDao::findEdge( type_t objType, oid_t src, oid_t tgt )
{
    oid_t eid = Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        eid = m_g->FindEdge(objType, src, tgt);
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "AbstractDao::findEdge: " << e.Message();
    }
#endif
    return eid;
}

bool AbstractDao::removeEdge( type_t edgeType, oid_t src, oid_t tgt )
{
    oid_t id = findEdge(edgeType, src, tgt);
    return removeEdge(edgeType, id);
}

bool AbstractDao::removeEdge( type_t edgeType, oid_t id )
{
    if( id == Objects::InvalidOID ) {
        LOG(logERROR) << "AbstractDao::removeEdgeImpl: invalid oid";
        return false;
    }
#ifdef MLD_SAFE

    try {
        auto t = m_g->GetObjectType(id);
        if( edgeType != t || t == Type::InvalidType ) {
            LOG(logERROR) << "AbstractDao::removeEdgeImpl: type mismatch";
            return false;
        }
#endif
        m_g->Drop(id);
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "AbstractDao::removeEdgeImpl: " << e.Message();
        return false;
    }
#endif
    return true;
}
