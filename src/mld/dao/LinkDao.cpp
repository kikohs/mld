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
#include "mld/dao/LinkDao.h"

using namespace mld;

typedef std::unique_ptr<dex::gdb::Objects> ObjectsPtr;

LinkDao::LinkDao( dex::gdb::Graph* g )
    : AbstractDao(g)
    , m_hType(m_g->FindType(EdgeType::H_LINK))
    , m_vType(m_g->FindType(EdgeType::V_LINK))
{
}

LinkDao::~LinkDao()
{
}

HLink LinkDao::addHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
#ifdef MLD_SAFE
    if( src == tgt ) { // no self loop
        LOG(logWARNING) << "LinkDao::addHLink: no self-loop allowed";
        return HLink();
    }

    // no invalid edge
    if( src == dex::gdb::Objects::InvalidOID || tgt == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::addHLink: source or target is invalid";
        return HLink();
    }
#endif

    auto id = m_g->NewEdge(m_hType, src, tgt);
    HLink res(src, tgt);
    res.setId(id);
    return res;
}

HLink LinkDao::addHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight )
{
    HLink res = addHLink(src, tgt);

#ifdef MLD_SAFE
    if( res.id() == dex::gdb::Objects::InvalidOID )
        return HLink();
#endif
    // Set weight attribute
    m_g->SetAttribute(res.id(), m_g->FindAttribute(m_hType, H_LinkAttr::WEIGHT), m_v->SetDouble(weight));
    res.setWeight(weight);
    return res;
}

VLink LinkDao::addVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
#ifdef MLD_SAFE
    if( src == tgt ) { // no self loop
        LOG(logWARNING) << "LinkDao::addVLink: no self-loop allowed";
        return VLink();
    }

    // no invalid edge
    if( src == dex::gdb::Objects::InvalidOID || tgt == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::addVLink: source or target is invalid";
        return VLink();
    }
#endif

    auto id = m_g->NewEdge(m_vType, src, tgt);
    VLink res(src, tgt);
    res.setId(id);
    return res;
}

VLink LinkDao::addVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight )
{
    VLink res = addVLink(src, tgt);
#ifdef MLD_SAFE
    if( res.id() == dex::gdb::Objects::InvalidOID )
        return VLink();
#endif
    // Set weight attribute
    m_g->SetAttribute(res.id(), m_g->FindAttribute(m_vType, V_LinkAttr::WEIGHT), m_v->SetDouble(weight));
    res.setWeight(weight);
    return res;
}

HLink LinkDao::getHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    ObjectsPtr obj(m_g->Edges(m_hType, src, tgt));
#ifdef MLD_SAFE
    if( obj->Count() == 0 ) {
        LOG(logERROR) << "LinkDao::getHLink: edge doesn't exist: " << src << " " << tgt;
        return HLink();
    }
#endif
    HLink hlink(src, tgt);
    hlink.setId(obj->Any());
    hlink.setWeight(getWeight(m_hType, hlink.id()));
    return hlink;
}

HLink LinkDao::getHLink( dex::gdb::oid_t hid )
{
#ifdef MLD_SAFE
    if( hid == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getHLink: invalid oid";
        return HLink();
    }
#endif
    std::unique_ptr<dex::gdb::EdgeData> data(m_g->GetEdgeData(hid));
    HLink hlink(data->GetTail(), data->GetHead());
    hlink.setId(hid);
    hlink.setWeight(getWeight(m_hType, hid));
    return hlink;
}

VLink LinkDao::getVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    ObjectsPtr obj(m_g->Edges(m_vType, src, tgt));
#ifdef MLD_SAFE
    if( obj->Count() == 0 ) {
        LOG(logERROR) << "LinkDao::getVLink: edge doesn't exist: " << src << " " << tgt;
        return VLink();
    }
#endif
    VLink vlink(src, tgt);
    vlink.setId(obj->Any());
    vlink.setWeight(getWeight(m_vType, vlink.id()));
    return vlink;
}

VLink LinkDao::getVLink( dex::gdb::oid_t vid )
{
#ifdef MLD_SAFE
    if( vid == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getVLink: invalid oid";
        return VLink();
    }
#endif
    std::unique_ptr<dex::gdb::EdgeData> data(m_g->GetEdgeData(vid));
    VLink vlink(data->GetTail(), data->GetHead());
    vlink.setId(vid);
    vlink.setWeight(getWeight(m_vType, vid));
    return vlink;
}

bool LinkDao::removeHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    return removeLinkImpl(m_hType, src, tgt);
}

bool LinkDao::removeHLink( dex::gdb::oid_t hid )
{
    return removeLinkImpl(m_hType, hid);
}

bool LinkDao::removeVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    return removeLinkImpl(m_vType, src, tgt);
}

bool LinkDao::removeVLink( dex::gdb::oid_t vid )
{
    return removeLinkImpl(m_vType, vid);
}

bool LinkDao::updateHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight )
{
    ObjectsPtr obj(m_g->Edges(m_hType, src, tgt));
#ifdef MLD_SAFE
    if( obj->Count() == 0 ) {
        LOG(logERROR) << "LinkDao::updateHLink: edge doesn't exist: " << src << " " << tgt;
        return false;
    }
#endif
    return updateHLink(obj->Any(), weight);
}

bool LinkDao::updateHLink( dex::gdb::oid_t hid, double weight )
{
#ifdef MLD_SAFE
    if( hid == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getHLink: invalid oid";
        return false;
    }
#endif
    setWeight(m_vType, hid, weight);
    return true;
}

bool LinkDao::updateVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight )
{
    ObjectsPtr obj(m_g->Edges(m_vType, src, tgt));
#ifdef MLD_SAFE
    if( obj->Count() == 0 ) {
        LOG(logERROR) << "LinkDao::updateVLink: edge doesn't exist: " << src << " " << tgt;
        return false;
    }
#endif
    return updateVLink(obj->Any(), weight);
}

bool LinkDao::updateVLink( dex::gdb::oid_t vid, double weight )
{
#ifdef MLD_SAFE
    if( vid == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getVLink: invalid oid";
        return false;
    }
#endif
    setWeight(m_vType, vid, weight);
    return true;
}

// ****** PRIVATE METHODS ****** //

double LinkDao::getWeight( dex::gdb::type_t edgeType, dex::gdb::oid_t id )
{
    double weight = 0;
    if( edgeType == m_hType ) {
        auto t = m_g->FindAttribute(m_hType, H_LinkAttr::WEIGHT);
        weight = m_g->GetAttribute(id, t)->GetDouble();
    }
    else if( edgeType == m_vType ) {
        auto t = m_g->FindAttribute(m_hType, V_LinkAttr::WEIGHT);
        weight = m_g->GetAttribute(id, t)->GetDouble();
    }
    else {
        LOG(logERROR) << "LinkDao::getWeight: unsupported edge type";
    }
    return weight;
}

void LinkDao::setWeight( dex::gdb::type_t edgeType, dex::gdb::oid_t id , double weight )
{
    if( edgeType == m_hType ) {
        auto t = m_g->FindAttribute(m_hType, H_LinkAttr::WEIGHT);
        m_g->SetAttribute(id, t, m_v->SetDouble(weight));
    }
    else if( edgeType == m_vType ) {
        auto t = m_g->FindAttribute(m_hType, V_LinkAttr::WEIGHT);
        m_g->SetAttribute(id, t, m_v->SetDouble(weight));
    }
    else {
        LOG(logERROR) << "LinkDao::setWeight: unsupported edge type";
    }
}

bool LinkDao::removeLinkImpl( dex::gdb::type_t edgeType, dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    ObjectsPtr obj(m_g->Edges(edgeType, src, tgt));
#ifdef MLD_SAFE
    if( obj->Count() == 0 ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: edge doesn't exist: " << src << " " << tgt;
        return false;
    }
#endif
    m_g->Drop(obj.get());
    return true;
}

bool LinkDao::removeLinkImpl( dex::gdb::type_t edgeType, dex::gdb::oid_t id )
{
#ifdef MLD_SAFE
    if( id == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: invalid oid";
        return false;
    }
    if( edgeType != m_g->GetObjectType(id) ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: type mismatch";
        return false;
    }
#endif
    m_g->Drop(id);
    return true;
}


