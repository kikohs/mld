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

#include "mld/GraphTypes.h"
#include "mld/dao/NodeDao.h"

using namespace mld;
using namespace sparksee::gdb;

NodeDao::NodeDao( Graph* g )
    : AbstractDao(g)
{
    if( g )
        m_nType = m_g->FindType(NodeType::NODE);
}

NodeDao::~NodeDao()
{
    // DO NOT DELETE GRAPH
}

void NodeDao::setGraph( Graph* g )
{
    if( g ) {
        AbstractDao::setGraph(g);
        m_nType = m_g->FindType(NodeType::NODE);
    }
}

mld::Node NodeDao::addNode()
{
    return Node( m_g->NewNode(m_nType) );
}

void NodeDao::removeNode( oid_t id )
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
        LOG(logERROR) << "NodeDao::removeNode " << e.Message();
    }
#endif
}

void NodeDao::updateNode( Node& n )
{
    updateAttrMap(m_nType, n.id(), n.data());
}

mld::Node NodeDao::getNode( oid_t id )
{
#ifdef MLD_SAFE
    if( id == Objects::InvalidOID )
        return Node();
#endif

    AttrMap data;
#ifdef MLD_SAFE
    try {
#endif
        data = readAttrMap(id);
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "NodeDao::getNode: " << e.Message();
        return Node();
    }
#endif
    return Node(id, data);
}


std::vector<mld::Node> NodeDao::getNode( const ObjectsPtr& objs )
{
    std::vector<Node> res;
    if( !objs )
        return res;

    res.reserve(objs->Count());
    ObjectsIt it(objs->Iterator());
    while( it->HasNext() ) {
        Node n = getNode(it->Next());
#ifdef MLD_SAFE
        // Watch for the if, no { }
        if( n.id() != Objects::InvalidOID )
#endif
            res.push_back(n);
    }
    return res;
}





