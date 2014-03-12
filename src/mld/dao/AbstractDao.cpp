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
