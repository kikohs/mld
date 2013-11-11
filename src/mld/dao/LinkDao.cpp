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
#include "mld/dao/LinkDao.h"

using namespace mld;

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
    dex::gdb::oid_t hid = dex::gdb::Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        hid = m_g->NewEdge(m_hType, src, tgt);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& ) {
        LOG(logERROR) << "LinkDao::addHLink: invalid src or tgt";
        return HLink();
    }
#endif
    HLink res(src, tgt);
    res.setId(hid);
    return res;
}

HLink LinkDao::addHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight )
{
    HLink res = addHLink(src, tgt);

#ifdef MLD_SAFE
    if( res.id() == dex::gdb::Objects::InvalidOID )
        return HLink();

    if( weight < 0.0 ) {
        LOG(logERROR) << "LinkDao:addHLink: weight is < 0.0";
        return HLink();
    }
#endif
    // Set weight attribute
    m_g->SetAttribute(res.id(), m_g->FindAttribute(m_hType, H_LinkAttr::WEIGHT), m_v->SetDouble(weight));
    res.setWeight(weight);
    return res;
}

VLink LinkDao::addVLink( dex::gdb::oid_t child, dex::gdb::oid_t parent )
{
#ifdef MLD_SAFE
    if( child == parent ) { // no self loop
        LOG(logWARNING) << "LinkDao::addVLink: no self-loop allowed";
        return VLink();
    }

    // no invalid edge
    if( child == dex::gdb::Objects::InvalidOID || parent == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::addVLink: source or target is invalid";
        return VLink();
    }
#endif
    dex::gdb::oid_t vid = dex::gdb::Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        vid = m_g->NewEdge(m_vType, child, parent);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& ) {
        LOG(logERROR) << "LinkDao::addVLink: invalid src or tgt";
        return VLink();
    }
#endif
    VLink res(child, parent);
    res.setId(vid);
    return res;
}

VLink LinkDao::addVLink( dex::gdb::oid_t src, dex::gdb::oid_t parent, double weight )
{
    VLink res = addVLink(src, parent);
#ifdef MLD_SAFE
    if( res.id() == dex::gdb::Objects::InvalidOID )
        return VLink();

    if( weight < 0.0 ) {
        LOG(logERROR) << "LinkDao:addVLink: weight is < 0.0";
        return VLink();
    }
#endif
    // Set weight attribute
    m_g->SetAttribute(res.id(), m_g->FindAttribute(m_vType, V_LinkAttr::WEIGHT), m_v->SetDouble(weight));
    res.setWeight(weight);
    return res;
}

HLink LinkDao::getHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    dex::gdb::oid_t hid = dex::gdb::Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        hid = m_g->FindEdge(m_hType, src, tgt);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "LinkDao::getHLink: " << e.Message();
        return HLink();
    }

    if( hid == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getHLink: edge doesn't exist: " << src << " " << tgt;
        return HLink();
    }
#endif
    HLink hlink(src, tgt);
    hlink.setId(hid);
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
    std::unique_ptr<dex::gdb::EdgeData> data;
#ifdef MLD_SAFE
    try {
        if( m_g->GetObjectType(hid) != m_hType ) {
            LOG(logERROR) << "LinkDao::getHLink: invalid type";
            return HLink();
        }
#endif
        data.reset(m_g->GetEdgeData(hid));
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "LinkDao::getHLink: " << e.Message();
        return HLink();
    }
#endif
    HLink hlink(data->GetTail(), data->GetHead());
    hlink.setId(hid);
    hlink.setWeight(getWeight(m_hType, hid));
    return hlink;
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
        if( e.id() != dex::gdb::Objects::InvalidOID )
#endif
            res.push_back(e);
    }
    return res;
}

VLink LinkDao::getVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    dex::gdb::oid_t vid = dex::gdb::Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
    vid = m_g->FindEdge(m_vType, src, tgt);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "LinkDao::getVLink: " << e.Message();
        return VLink();
    }

    if( vid == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getVLink: edge doesn't exist: " << src << " " << tgt;
        return VLink();
    }
#endif
    VLink vlink(src, tgt);
    vlink.setId(vid);
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
    std::unique_ptr<dex::gdb::EdgeData> data;
#ifdef MLD_SAFE
    try {
        if( m_g->GetObjectType(vid) != m_vType ) {
            LOG(logERROR) << "LinkDao::getVLink: invalid type";
            return VLink();
        }
#endif
        data.reset(m_g->GetEdgeData(vid));
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "LinkDao::getVLink: " << e.Message();
        return VLink();
    }
#endif
    VLink vlink(data->GetTail(), data->GetHead());
    vlink.setId(vid);
    vlink.setWeight(getWeight(m_vType, vid));
    return vlink;
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
        if( e.id() != dex::gdb::Objects::InvalidOID )
#endif
            res.push_back(e);
    }
    return res;
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
    dex::gdb::oid_t hid = dex::gdb::Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        hid = m_g->FindEdge(m_hType, src, tgt);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& ) {
        LOG(logERROR) << "LinkDao::updateHLink: invalid src or tgt";
        return false;
    }
#endif
    return updateHLink(hid, weight);
}

bool LinkDao::updateHLink( dex::gdb::oid_t hid, double weight )
{
#ifdef MLD_SAFE
    if( hid == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getHLink: invalid oid";
        return false;
    }
#endif
    return setWeight(m_hType, hid, weight);
}

bool LinkDao::updateVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight )
{
    dex::gdb::oid_t vid = dex::gdb::Objects::InvalidOID;
 #ifdef MLD_SAFE
     try {
 #endif
         vid = m_g->FindEdge(m_vType, src, tgt);
 #ifdef MLD_SAFE
     } catch( dex::gdb::Error& ) {
         LOG(logERROR) << "LinkDao::updateVLink: invalid src or tgt";
     }
 #endif
    return updateVLink(vid, weight);
}

bool LinkDao::updateVLink( dex::gdb::oid_t vid, double weight )
{
#ifdef MLD_SAFE
    if( vid == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::getVLink: invalid oid";
        return false;
    }
#endif
    return setWeight(m_vType, vid, weight);
}

// ****** PRIVATE METHODS ****** //

double LinkDao::getWeight( dex::gdb::type_t edgeType, dex::gdb::oid_t id )
{
    double weight = 0;
    dex::gdb::attr_t attr;
    if( edgeType == m_hType )
        attr = m_g->FindAttribute(m_hType, H_LinkAttr::WEIGHT);
    else
        attr = m_g->FindAttribute(m_vType, V_LinkAttr::WEIGHT);

#ifdef MLD_SAFE
    try {
#endif
        weight = m_g->GetAttribute(id, attr)->GetDouble();
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "LinkDao::getWeight: " << e.Message();
    }
#endif
    return weight;
}

bool LinkDao::setWeight( dex::gdb::type_t edgeType, dex::gdb::oid_t id , double weight )
{
    dex::gdb::attr_t attr;
    if( edgeType == m_hType )
        attr = m_g->FindAttribute(m_hType, H_LinkAttr::WEIGHT);
    else
        attr = m_g->FindAttribute(m_vType, V_LinkAttr::WEIGHT);

#ifdef MLD_SAFE
    try {
#endif
        m_g->SetAttribute(id, attr, m_v->SetDouble(weight));
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "LinkDao::setWeight: " << e.Message();
        return false;
    }
#endif
    return true;
}

bool LinkDao::removeLinkImpl( dex::gdb::type_t edgeType, dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    dex::gdb::oid_t id = dex::gdb::Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        id = m_g->FindEdge(edgeType, src, tgt);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: invalid src and or target";
        return false;
    }
#endif
    return removeLinkImpl(edgeType, id);
}

bool LinkDao::removeLinkImpl( dex::gdb::type_t edgeType, dex::gdb::oid_t id )
{
#ifdef MLD_SAFE
    if( id == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: invalid oid";
        return false;
    }

    try {
        auto t = m_g->GetObjectType(id);
        if( edgeType != t || t == dex::gdb::Type::InvalidType ) {
            LOG(logERROR) << "LinkDao::removeLinkImpl: type mismatch";
            return false;
        }
#endif
        m_g->Drop(id);
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "LinkDao::removeLinkImpl: " << e.Message();
        return false;
    }
#endif
    return true;
}


