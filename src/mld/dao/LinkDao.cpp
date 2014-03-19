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
        m_hType = m_g->FindType(EdgeType::HLINK);
        m_vType = m_g->FindType(EdgeType::VLINK);
        m_oType = m_g->FindType(EdgeType::OLINK);
        auto layerType = m_g->FindType(NodeType::LAYER);
        auto nType = m_g->FindType(NodeType::NODE);

        // Create 2 nodes to get in cache hLinkAttrMap and vLinkAttrMap
        auto nid = m_g->NewNode(nType);
        auto nid2 = m_g->NewNode(nType);
        auto hid = m_g->NewEdge(m_hType, nid, nid2);
        // Read default hLink attributes map
        m_hLinkAttr = readAttrMap(hid);

        auto vid = m_g->NewEdge(m_vType, nid, nid2);
        // Read default vLink attributes map
        m_vLinkAttr = readAttrMap(vid);

        // Read default oLink attributes map
        auto l = m_g->NewNode(layerType);
        auto ownsId = m_g->NewEdge(m_oType, l, nid);
        m_oLinkAttr = readAttrMap(ownsId);
        // Not needed anymore
        m_g->Drop(l);
        m_g->Drop(nid);
        m_g->Drop(nid2);
    }
}


// ***** HLINK ***** //


HLink LinkDao::addHLink( oid_t src, oid_t tgt )
{
    return addLink<HLink>(m_hType, src, tgt, m_hLinkAttr, false);
}

HLink LinkDao::addHLink( oid_t src, oid_t tgt, double weight )
{
    AttrMap data(m_hLinkAttr);
    data[Attrs::V[HLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return addLink<HLink>(m_hType, src, tgt, data, true);
}

HLink LinkDao::addHLink( oid_t src, oid_t tgt, AttrMap& data )
{
    return addLink<HLink>(m_hType, src, tgt, data, true);
}

HLink LinkDao::getHLink( oid_t src, oid_t tgt )
{
    return getLink<HLink>(m_hType, src, tgt);
}

HLink LinkDao::getHLink( oid_t hid )
{
    return getLink<HLink>(m_hType, hid);
}

std::vector<HLink> LinkDao::getHLink( const ObjectsPtr& objs )
{
    return getLink<HLink>(m_hType, objs);
}

bool LinkDao::updateHLink( oid_t src, oid_t tgt, double weight )
{
    AttrMap data;
    data[Attrs::V[HLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return updateAttrMap(m_hType, findEdge(m_hType, src, tgt), data);
}

bool LinkDao::updateHLink( oid_t src, oid_t tgt, AttrMap& data )
{
    return updateAttrMap(m_hType, findEdge(m_hType, src, tgt), data);
}

bool LinkDao::updateHLink( oid_t hid, AttrMap& data )
{
    return updateAttrMap(m_hType, hid, data);
}

bool LinkDao::updateHLink( oid_t hid, double weight )
{
    AttrMap data;
    data[Attrs::V[HLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return updateAttrMap(m_hType, hid, data);
}

bool LinkDao::removeHLink( oid_t src, oid_t tgt )
{
    return removeEdge(m_hType, src, tgt);
}

bool LinkDao::removeHLink( oid_t hid )
{
    return removeEdge(m_hType, hid);
}


// ***** VLINK ***** //


VLink LinkDao::addVLink( oid_t child, oid_t parent )
{
    return addLink<VLink>(m_vType, child, parent, m_vLinkAttr, false);
}

VLink LinkDao::addVLink( oid_t child, oid_t parent, double weight )
{
    AttrMap data(m_vLinkAttr);
    data[Attrs::V[VLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return addLink<VLink>(m_vType, child, parent, data, true);
}

VLink LinkDao::addVLink( oid_t child, oid_t parent, AttrMap& data )
{
    return addLink<VLink>(m_vType, child, parent, data, true);
}

VLink LinkDao::getVLink( oid_t child, oid_t parent )
{
    return getLink<VLink>(m_vType, child, parent);
}

VLink LinkDao::getVLink( oid_t vid )
{
    return getLink<VLink>(m_vType, vid);
}

std::vector<VLink> LinkDao::getVLink( const ObjectsPtr& objs )
{
    return getLink<VLink>(m_vType, objs);
}

bool LinkDao::updateVLink( oid_t child, oid_t parent, double weight )
{
    AttrMap data;
    data[Attrs::V[VLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return updateAttrMap(m_vType, findEdge(m_vType, child, parent), data);
}

bool LinkDao::updateVLink( oid_t child, oid_t parent, AttrMap& data )
{
    return updateAttrMap(m_vType, findEdge(m_vType, child, parent), data);
}

bool LinkDao::updateVLink( oid_t eid, AttrMap& data )
{
    return updateAttrMap(m_vType, eid, data);
}

bool LinkDao::updateVLink( oid_t vid, double weight )
{
    AttrMap data;
    data[Attrs::V[VLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return updateAttrMap(m_vType, vid, data);
}

bool LinkDao::removeVLink( oid_t src, oid_t tgt )
{
    return removeEdge(m_vType, src, tgt);
}

bool LinkDao::removeVLink( oid_t vid )
{
    return removeEdge(m_vType, vid);
}


// ***** OLINK ***** //


OLink LinkDao::addOLink( oid_t layer, oid_t node )
{
    return addLink<OLink>(m_oType, layer, node, m_oLinkAttr, false);
}

OLink LinkDao::addOLink( oid_t layer, oid_t node, double weight )
{
    AttrMap data(m_oLinkAttr);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return addLink<OLink>(m_oType, layer, node, data, true);
}

OLink LinkDao::addOLink( oid_t layer, oid_t node, AttrMap& data )
{
    return addLink<OLink>(m_oType, layer, node, data, true);
}

OLink LinkDao::getOLink( oid_t layer, oid_t node )
{
    return getLink<OLink>(m_oType, layer, node);
}

OLink LinkDao::getOLink( oid_t eid )
{
    return getLink<OLink>(m_oType, eid);
}

std::vector<OLink> LinkDao::getOLink( const ObjectsPtr& objs )
{
    return getLink<OLink>(m_oType, objs);
}

bool LinkDao::updateOLink( oid_t layer, oid_t node, double weight )
{
    AttrMap data(m_oLinkAttr);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return updateAttrMap(m_oType, findEdge(m_oType, layer, node), data);
}

bool LinkDao::updateOLink( oid_t layer, oid_t node, AttrMap& data )
{
    return updateAttrMap(m_oType, findEdge(m_oType, layer, node), data);
}

bool LinkDao::updateOLink( oid_t eid, AttrMap& data )
{
    return updateAttrMap(m_oType, eid, data);
}

bool LinkDao::updateOLink( oid_t eid, double weight )
{
    AttrMap data;
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return updateAttrMap(m_oType, eid, data);
}

bool LinkDao::removeOLink( oid_t src, oid_t tgt )
{
    return removeEdge(m_oType, src, tgt);
}

bool LinkDao::removeOLink( oid_t eid )
{
    return removeEdge(m_oType, eid);
}

