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
#include <sparksee/gdb/Graph_data.h>
#include <sparksee/gdb/Value.h>

#include "mld/GraphTypes.h"
#include "mld/dao/SNodeDao.h"

using namespace mld;
using namespace sparksee::gdb;

SNodeDao::SNodeDao( Graph* g )
    : AbstractDao(g)
{
    if( g )
        m_snType = m_g->FindType(NodeType::SUPERNODE);
}

SNodeDao::~SNodeDao()
{
    // DO NOT DELETE GRAPH
}

void SNodeDao::setGraph( Graph* g )
{
    if( g ) {
        AbstractDao::setGraph(g);
        m_snType = m_g->FindType(NodeType::SUPERNODE);
    }
}

SuperNode SNodeDao::addNode()
{
    return SuperNode( m_g->NewNode(m_snType) );
}

void SNodeDao::removeNode( oid_t id )
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
    } catch( Error& e ) {
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
    // WEIGHT
    attr_t att = m_g->FindAttribute(m_snType, SNAttr::WEIGHT);
    m_v->SetDoubleVoid(n.weight());
    m_g->SetAttribute(n.id(), att, *m_v);
    // LABEL
    att = m_g->FindAttribute(m_snType, SNAttr::LABEL);
    m_v->SetStringVoid(n.label());
    m_g->SetAttribute(n.id(), att, *m_v);
//    // IS_ROOT
//    att = m_g->FindAttribute(m_snType, SNAttr::IS_ROOT);
//    m_v->SetBooleanVoid(n.isRoot());
//    m_g->SetAttribute(n.id(), att, *m_v);
}

SuperNode SNodeDao::getNode( oid_t id )
{
#ifdef MLD_SAFE
    if( id == Objects::InvalidOID )
        return SuperNode();
#endif

    double w = 0.0;
    std::wstring lab = L"";
#ifdef MLD_SAFE
    try {
#endif
        // WEIGHT
        m_g->GetAttribute(id, m_g->FindAttribute(m_snType, SNAttr::WEIGHT), *m_v);
        w = m_v->GetDouble();
        // LABEL
        m_g->GetAttribute(id, m_g->FindAttribute(m_snType, SNAttr::LABEL), *m_v);
        lab = m_v->GetString();
//        // IS_ROOT
//        m_g->GetAttribute(id, m_g->FindAttribute(m_snType, SNAttr::IS_ROOT), *m_v);
//        isRoot = m_v->GetBoolean();
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "SNodeDao::getNode: " << e.Message();
        return SuperNode();
    }
#endif
    return SuperNode(id, lab, w);
}


std::vector<SuperNode> SNodeDao::getNode( const ObjectsPtr& objs )
{
    std::vector<SuperNode> res;
    if( !objs )
        return res;

    res.reserve(objs->Count());
    ObjectsIt it(objs->Iterator());
    while( it->HasNext() ) {
        SuperNode n = getNode(it->Next());
#ifdef MLD_SAFE
        // Watch for the if, no { }
        if( n.id() != Objects::InvalidOID )
#endif
            res.push_back(n);
    }
    return res;
}





