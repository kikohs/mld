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
    setGraph(g);
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
        // Create a node to get in cache nodeAttrMap
        auto nid = m_g->NewNode(m_nType);
        m_nodeAttr = readAttrMap(nid);
        // Not needed anymore
        m_g->Drop(nid);
    }
}

mld::Node NodeDao::addNode()
{
    return Node(m_g->NewNode(m_nType), m_nodeAttr);
}

mld::Node NodeDao::addNode( double weight )
{
    AttrMap data(m_nodeAttr);
    data[NodeAttr::WEIGHT].SetDoubleVoid(weight);
    return addNode(data);
}

mld::Node NodeDao::addNode( AttrMap& data )
{
    AttrMap dat(m_nodeAttr);
    // Add or update the attrMap that will be persisted
    for( auto& kv: data )
        dat[kv.first] = kv.second;
    Node n(m_g->NewNode(m_nType), dat);
    updateAttrMap(m_nType, n.id(), n.data());
    return n;
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

bool NodeDao::updateNode( Node& n )
{
    return updateAttrMap(m_nType, n.id(), n.data());
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





