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
#include <sparksee/gdb/Graph_data.h>
#include <sparksee/gdb/Value.h>

#include "mld/GraphTypes.h"
#include "mld/utils/log.h"
#include "mld/dao/OwnsDao.h"

using namespace mld;
using namespace sparksee::gdb;

OwnsDao::OwnsDao( Graph* g )
    : AbstractDao(g)
{
    setGraph(g);
}

OwnsDao::~OwnsDao()
{
    // DO NOT DELETE GRAPH
}

void OwnsDao::setGraph( Graph* g )
{
    if( g ) {
        AbstractDao::setGraph(g);
        m_ownsType = m_g->FindType(EdgeType::OWNS);
        auto layerType = m_g->FindType(NodeType::LAYER);
        auto nType = m_g->FindType(NodeType::NODE);
        // Create 2 nodes to get in cache ownsAttrMap
        auto l = m_g->NewNode(layerType);
        auto node = m_g->NewNode(nType);
        auto ownsId = m_g->NewEdge(m_ownsType, l, node);
        m_ownsAttr = readAttrMap(ownsId);
        m_g->Drop(l);
        m_g->Drop(node);
    }
}

OLink OwnsDao::addOLink( oid_t layerId, oid_t nodeId )
{
    oid_t eid = addEdge(m_ownsType, layerId, nodeId);
    if( eid == Objects::InvalidOID )
        return OLink();
    return OLink(eid, layerId, nodeId, m_ownsAttr);
}

OLink OwnsDao::addOLink( oid_t layerId, oid_t nodeId, AttrMap& data )
{
    oid_t eid = addEdge(m_ownsType, layerId, nodeId);
    if( eid == Objects::InvalidOID )
        return OLink();
    if( !updateAttrMap(m_ownsType, eid, data) )
        return OLink();
    return OLink(eid, layerId, nodeId, data);
}

void OwnsDao::removeOLink( oid_t layerId, oid_t nodeId )
{
    oid_t eid = findEdge(m_ownsType, layerId, nodeId);
    if( eid != Objects::InvalidOID ) {
        m_g->Drop(eid);
    }
}

OLink OwnsDao::getOLink( oid_t layerId, oid_t nodeId )
{
    oid_t eid = findEdge(m_ownsType, layerId, nodeId);
    if( eid == Objects::InvalidOID )
        return OLink();
    return OLink(eid, layerId, nodeId, readAttrMap(eid));
}

OLink OwnsDao::getOLink( oid_t ownsId )
{
#ifdef MLD_SAFE
    if( ownsId == Objects::InvalidOID ) {
        LOG(logERROR) << "OwnsDao::getOLink: invalid oid";
        return OLink();
    }
#endif
    std::unique_ptr<EdgeData> eData;
#ifdef MLD_SAFE
    try {
        if( m_g->GetObjectType(ownsId) != m_ownsType ) {
            LOG(logERROR) << "OwnsDao::getOLink: invalid type";
            return OLink();
        }
#endif
        eData.reset(m_g->GetEdgeData(ownsId));
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "OwnsDao::getOLink: " << e.Message();
        return OLink();
    }
#endif
    return OLink(ownsId, eData->GetTail(), eData->GetHead(), readAttrMap(ownsId));
}
