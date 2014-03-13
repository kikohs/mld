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
#include "mld/dao/LinkDao.h"

using namespace mld;
using namespace sparksee::gdb;

LinkDao::LinkDao( Graph* g )
    : AbstractDao(g)
{
    setGraph(g);
}

LinkDao::~LinkDao()
{
}

void LinkDao::setGraph( Graph* g )
{
    if( g ) {
        AbstractDao::setGraph(g);
        m_hType = m_g->FindType(EdgeType::H_LINK);
        m_vType = m_g->FindType(EdgeType::V_LINK);

        auto nType = m_g->FindType(NodeType::NODE);
        // Create 2 nodes to get in cache hLinkAttrMap and vLinkAttrMap
        auto nid = m_g->NewNode(nType);
        auto nid2 = m_g->NewNode(nType);
        auto hid = m_g->NewEdge(m_hType, nid, nid2);
        m_hLinkAttr = readAttrMap(hid);
        auto vid = m_g->NewEdge(m_vType, nid, nid2);
        m_vLinkAttr = readAttrMap(vid);
        // Not needed anymore
        m_g->Drop(nid);
        m_g->Drop(nid2);
    }
}

HLink LinkDao::addHLink( oid_t src, oid_t tgt )
{
    oid_t eid = addEdge(m_hType, src, tgt);
    if( eid == Objects::InvalidOID )
        return HLink();
    return HLink(eid, src, tgt, m_hLinkAttr);
}

HLink LinkDao::addHLink( oid_t src, oid_t tgt, double weight )
{
    AttrMap data(m_hLinkAttr);
    data[H_LinkAttr::WEIGHT].SetDoubleVoid(weight);
    return addHLink(src, tgt, data);
}

HLink LinkDao::addHLink( oid_t src, oid_t tgt, AttrMap& data )
{
    oid_t eid = addEdge(m_hType, src, tgt);
    if( eid == Objects::InvalidOID )
        return HLink();
    if( !updateAttrMap(m_hType, eid, data) )
        return HLink();
    return HLink(eid, src, tgt, data);
}

VLink LinkDao::addVLink( oid_t child, oid_t parent )
{
    oid_t eid = addEdge(m_vType, child, parent);
    if( eid == Objects::InvalidOID )
        return VLink();
    return VLink(eid, child, parent, m_vLinkAttr);
}

VLink LinkDao::addVLink( oid_t child, oid_t parent, double weight )
{
    AttrMap data(m_vLinkAttr);
    data[V_LinkAttr::WEIGHT].SetDoubleVoid(weight);
    return addVLink(child, parent, data);
}

VLink LinkDao::addVLink( oid_t child, oid_t parent, AttrMap& data )
{
    oid_t eid = addEdge(m_vType, child, parent);
    if( eid == Objects::InvalidOID )
        return VLink();
    if( !updateAttrMap(m_vType, eid, data) )
        return VLink();
    return VLink(eid, child, parent, data);
}

HLink LinkDao::getHLink( oid_t src, oid_t tgt )
{
    oid_t eid = Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        eid = m_g->FindEdge(m_hType, src, tgt);
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "LinkDao::getHLink: " << e.Message();
        return HLink();
    }
#endif
    if( eid == Objects::InvalidOID )
        return HLink();
    return HLink(eid, src, tgt, readAttrMap(eid));
}

HLink LinkDao::getHLink( oid_t hid )
{
#ifdef MLD_SAFE
    if( hid == Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getHLink: invalid oid";
        return HLink();
    }
#endif
    std::unique_ptr<EdgeData> eData;
#ifdef MLD_SAFE
    try {
        if( m_g->GetObjectType(hid) != m_hType ) {
            LOG(logERROR) << "LinkDao::getHLink: invalid type";
            return HLink();
        }
#endif
        eData.reset(m_g->GetEdgeData(hid));
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "LinkDao::getHLink: " << e.Message();
        return HLink();
    }
#endif
    return HLink(hid, eData->GetTail(), eData->GetHead(), readAttrMap(hid));
}

std::vector<HLink> LinkDao::getHLink( const ObjectsPtr& objs )
{
    std::vector<HLink> res;
    res.reserve(objs->Count());
    ObjectsIt it(objs->Iterator());
    while( it->HasNext() ) {
        HLink e = getHLink(it->Next());
#ifdef MLD_SAFE
        // Watch for the if, no { }
        if( e.id() != Objects::InvalidOID )
#endif
            res.push_back(e);
    }
    return res;
}

VLink LinkDao::getVLink( oid_t child, oid_t parent )
{
    oid_t vid = Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
    vid = m_g->FindEdge(m_vType, child, parent);
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "LinkDao::getVLink: " << e.Message();
        return VLink();
    }
#endif
    if( vid == Objects::InvalidOID )
        return VLink();
    return VLink(vid, child, parent, readAttrMap(vid));;
}

VLink LinkDao::getVLink( oid_t vid )
{
#ifdef MLD_SAFE
    if( vid == Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getVLink: invalid oid";
        return VLink();
    }
#endif
    std::unique_ptr<EdgeData> eData;
#ifdef MLD_SAFE
    try {
        if( m_g->GetObjectType(vid) != m_vType ) {
            LOG(logERROR) << "LinkDao::getVLink: invalid type";
            return VLink();
        }
#endif
        eData.reset(m_g->GetEdgeData(vid));
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "LinkDao::getVLink: " << e.Message();
        return VLink();
    }
#endif
    return VLink(vid, eData->GetTail(), eData->GetHead(), readAttrMap(vid));
}

std::vector<VLink> LinkDao::getVLink( const ObjectsPtr& objs )
{
    std::vector<VLink> res;
    res.reserve(objs->Count());
    ObjectsIt it(objs->Iterator());
    while( it->HasNext() ) {
        VLink e = getVLink(it->Next());
#ifdef MLD_SAFE
        // Watch for the if, no { }
        if( e.id() != Objects::InvalidOID )
#endif
            res.push_back(e);
    }
    return res;
}

bool LinkDao::removeHLink( oid_t src, oid_t tgt )
{
    return removeLinkImpl(m_hType, src, tgt);
}

bool LinkDao::removeHLink( oid_t hid )
{
    return removeLinkImpl(m_hType, hid);
}

bool LinkDao::removeVLink( oid_t src, oid_t tgt )
{
    return removeLinkImpl(m_vType, src, tgt);
}

bool LinkDao::removeVLink( oid_t vid )
{
    return removeLinkImpl(m_vType, vid);
}

bool LinkDao::updateHLink( oid_t src, oid_t tgt, double weight )
{
    AttrMap data;
    data[H_LinkAttr::WEIGHT].SetDoubleVoid(weight);
    return updateHLink(src, tgt, data);
}

bool LinkDao::updateHLink( oid_t src, oid_t tgt, AttrMap& data )
{
    oid_t eid = Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        eid = m_g->FindEdge(m_hType, src, tgt);
#ifdef MLD_SAFE
    } catch( Error& ) {
        LOG(logERROR) << "LinkDao::updateHLink: invalid src or tgt";
        return false;
    }
#endif
    return updateHLink(eid, data);
}

bool LinkDao::updateHLink( oid_t eid, AttrMap& data )
{
    return updateAttrMap(m_hType, eid, data);
}

bool LinkDao::updateHLink( oid_t hid, double weight )
{
    AttrMap data;
    data[H_LinkAttr::WEIGHT].SetDoubleVoid(weight);
    return updateHLink(hid, data);
}

bool LinkDao::updateVLink( oid_t child, oid_t parent, double weight )
{
    AttrMap data;
    data[V_LinkAttr::WEIGHT].SetDoubleVoid(weight);
    return updateVLink(child, parent, data);;
}

bool LinkDao::updateVLink( oid_t child, oid_t parent, AttrMap& data )
{
    oid_t eid = Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        eid = m_g->FindEdge(m_vType, child, parent);
#ifdef MLD_SAFE
    } catch( Error& ) {
        LOG(logERROR) << "LinkDao::updateVLink: invalid child or parent";
        return false;
    }
#endif
    return updateVLink(eid, data);
}

bool LinkDao::updateVLink( oid_t eid, AttrMap& data )
{
    return updateAttrMap(m_vType, eid, data);
}

bool LinkDao::updateVLink( oid_t vid, double weight )
{
    AttrMap data;
    data[V_LinkAttr::WEIGHT].SetDoubleVoid(weight);
    return updateVLink(vid, data);
}


// ****** PRIVATE METHODS ****** //
bool LinkDao::removeLinkImpl( type_t edgeType, oid_t src, oid_t tgt )
{
    oid_t id = Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        id = m_g->FindEdge(edgeType, src, tgt);
#ifdef MLD_SAFE
    } catch( Error& ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: invalid src and or target";
        return false;
    }
#endif
    return removeLinkImpl(edgeType, id);
}

bool LinkDao::removeLinkImpl( type_t edgeType, oid_t id )
{
#ifdef MLD_SAFE
    if( id == Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: invalid oid";
        return false;
    }

    try {
        auto t = m_g->GetObjectType(id);
        if( edgeType != t || t == Type::InvalidType ) {
            LOG(logERROR) << "LinkDao::removeLinkImpl: type mismatch";
            return false;
        }
#endif
        m_g->Drop(id);
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: " << e.Message();
        return false;
    }
#endif
    return true;
}

oid_t LinkDao::addEdge( type_t lType, oid_t src, oid_t tgt )
{
    oid_t eid = Objects::InvalidOID;
#ifdef MLD_SAFE
    if( src == tgt ) { // no self loop
        LOG(logWARNING) << "LinkDao::addEdge: no self-loop allowed";
        return eid;
    }

    // no invalid edge
    if( src == Objects::InvalidOID || tgt == Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::addEdge: source or target is invalid";
        return eid;
    }
#endif
#ifdef MLD_SAFE
    try {
#endif
        eid = m_g->NewEdge(lType, src, tgt);
#ifdef MLD_SAFE
    } catch( Error& ) {
        LOG(logERROR) << "LinkDao::addEdge: invalid src or tgt";
        return Objects::InvalidOID;
    }
#endif
    return eid;
}


