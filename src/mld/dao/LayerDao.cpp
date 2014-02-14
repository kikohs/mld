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
#include "mld/dao/LayerDao.h"
#include "mld/utils/log.h"

using namespace mld;
using namespace dex::gdb;

LayerDao::LayerDao( Graph* g )
    : AbstractDao(g)
    , m_lType(m_g->FindType(NodeType::LAYER))
{
}

LayerDao::~LayerDao()
{
    // DO NOT DELETE GRAPH
}

int64_t LayerDao::countLayers()
{
    std::unique_ptr<Objects> obj(m_g->Select(m_lType));
    return obj->Count();
}

Layer LayerDao::addLayerOnTop()
{
    auto base = baseLayerImpl();
    if( base == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::addLayerOnTop: need a base layer";
        return Layer();
    }
    Layer res;
    auto newId = addLayer();
    bool ok = attachOnTop(newId);
    if( ok ) // Valid attach
        res.m_id = newId;
    return res;
}

Layer LayerDao::addLayerOnBottom()
{
    auto base = baseLayerImpl();
    if( base == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::addLayerOnBottom: need a base layer";
        return Layer();
    }
    Layer res;
    auto newId = addLayer();
    bool ok = attachOnBottom(newId);
    if( ok ) // Valid attach
        res.m_id = newId;
    return res;
}

bool LayerDao::updateLayer( const Layer& layer )
{
    auto attrDes = m_g->FindAttribute(m_lType, LayerAttr::DESCRIPTION);
#ifdef MLD_SAFE
    try {
#endif
        m_g->SetAttribute(layer.id(), attrDes, m_v->SetString(layer.description()));
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "LayerDao::updateLayer: " << e.Message();
        return false;
    }
#endif
    return true;
}

Layer LayerDao::getLayer( oid_t id )
{
    if( id == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::getLayer: Get on invalid Layer oid";
        return Layer();
    }

    Layer res;
    res.m_id = id;
    auto attrDes = m_g->FindAttribute(m_lType, LayerAttr::DESCRIPTION);
    m_g->GetAttribute(id, attrDes, *m_v);
    res.setDescription(m_v->GetString());

    auto attrIsBase = m_g->FindAttribute(m_lType, LayerAttr::IS_BASE);
    m_g->GetAttribute(id, attrIsBase, *m_v);
    res.m_isBase = m_v->GetBoolean();

    return res;
}

bool LayerDao::removeTopLayer()
{
    auto top = topLayerImpl();
    auto base = baseLayerImpl();
    // If it is not the base layer, and it is valid, remove
    if( top != base && top != Objects::InvalidOID ) {
        return removeLayer(top);
    }
    return false;
}

bool LayerDao::removeBottomLayer()
{
    auto bot = bottomLayerImpl();
    auto base = baseLayerImpl();
    // If it is not the base layer, and it is valid, remove
    if( bot != base && bot != Objects::InvalidOID ) {
        return removeLayer(bot);
    }
    return false;
}

void LayerDao::setAsBaseLayer( Layer& layer )
{
    setAsBaseLayerImpl(layer.id());
    // Set private member variable
    layer.m_isBase = true;
}

Layer LayerDao::bottomLayer()
{
    return getLayer(bottomLayerImpl());
}

Layer LayerDao::topLayer()
{
    return getLayer(topLayerImpl());
}

Layer LayerDao::baseLayer()
{
    return getLayer(baseLayerImpl());
}

Layer LayerDao::parent( const Layer& layer )
{
    return getLayer(parentImpl(layer.id()));
}

Layer LayerDao::child( const Layer& layer )
{
    return getLayer(childImpl(layer.id()));
}

Layer LayerDao::addBaseLayer()
{
    auto id = baseLayerImpl();
    // No base layer, add
    if( id != Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::createBaseLayer: Base layer already exists";
        return Layer();
    }

    // No base layer add
    auto newid = addLayer();
    setAsBaseLayerImpl(newid);
    return getLayer(newid);
}

bool LayerDao::removeBaseLayer()
{
    auto nb = countLayers();
    auto base = baseLayerImpl();

    if( nb == 1 && base != Objects::InvalidOID )
        return removeLayer(base);

    return false;
}

bool LayerDao::removeAllButBaseLayer()
{
    bool success = true;
    // Remove all top layers
    while( success )
        success = removeTopLayer();

    // Remove all bottom layers
    success = true;
    while( success )
        success = removeBottomLayer();

    auto nb = countLayers();
    return nb == 1 ? true : false;
}

bool LayerDao::affiliated( oid_t layer1, oid_t layer2 )
{
    auto childType = m_g->FindType(EdgeType::CHILD_OF);
    oid_t eid = Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        // Check child_of
        eid = m_g->FindEdge(childType, layer1, layer2);
        if( eid == Objects::InvalidOID ) {
            // Check reverse child_of
            eid = m_g->FindEdge(childType, layer2, layer1);
            if( eid == Objects::InvalidOID )
                return false;
        }
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "LayerDao::affiliated: " << e.Message();
        return false;
    }
#endif
    return true;
}

// ********** PRIVATE METHODS ********** //
oid_t LayerDao::addLayer()
{
    return m_g->NewNode(m_lType);
}

bool LayerDao::removeLayer( oid_t lid )
{
    if( lid == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::removeLayer: Attempting to remove an invalid layer, abort";
        return false;
    }

    auto ownsType = m_g->FindType(EdgeType::OWNS);
    std::unique_ptr<Objects> nodesObj(m_g->Neighbors(lid, ownsType, Outgoing));
    // Remove all the associated nodes
    m_g->Drop(nodesObj.get());
    // Remove layer
    m_g->Drop(lid);
    return true;
}

bool LayerDao::attachOnTop( oid_t newId )
{
    auto top = topLayerImpl();
    if( top == Objects::InvalidOID )
        return false;

    auto childType = m_g->FindType(EdgeType::CHILD_OF);
    m_g->NewEdge(childType, top, newId);
    return true;
}

bool LayerDao::attachOnBottom( oid_t newId )
{
    auto bottom = bottomLayerImpl();
    if( bottom == Objects::InvalidOID )
        return false;

    auto childType = m_g->FindType(EdgeType::CHILD_OF);
    m_g->NewEdge(childType, newId, bottom);
    return true;
}

oid_t LayerDao::baseLayerImpl()
{
    auto attr = m_g->FindAttribute(m_lType, LayerAttr::IS_BASE);

    std::unique_ptr<Objects> obj(m_g->Select(attr, Equal, m_v->SetBoolean(true)));
    if( obj->Count() == 0 ) {
        return Objects::InvalidOID;
    }
    // Should only contain 1 element
    return obj->Any();
}

void LayerDao::setAsBaseLayerImpl( oid_t newId )
{
    auto attr = m_g->FindAttribute(m_lType, LayerAttr::IS_BASE);
    auto oldId = baseLayerImpl();
    // No base layer
    if( oldId == Objects::InvalidOID ) {
        m_g->SetAttribute(newId, attr, m_v->SetBoolean(true));
    }
    else { // There is already a base layer, switch
        m_g->SetAttribute(oldId, attr, m_v->SetBoolean(false));
        m_g->SetAttribute(newId, attr, m_v->SetBoolean(true));
    }
}

oid_t LayerDao::topLayerImpl()
{
    auto lid = baseLayerImpl();
    if( lid == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::topLayerImpl: Need a base layer";
        return Objects::InvalidOID;
    }

    bool stop = false;
    while( !stop ) {
        auto tmp = parentImpl(lid);
        if( tmp == Objects::InvalidOID )
            stop = true;
        else
            lid = tmp;
    }
    return lid;
}

oid_t LayerDao::bottomLayerImpl()
{
    auto lid = baseLayerImpl();
    if( lid == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::bottomLayerImpl: Need a base layer";
        return Objects::InvalidOID;
    }

    bool stop = false;
    while( !stop ) {
        auto tmp = childImpl(lid);
        if( tmp == Objects::InvalidOID )
            stop = true;
        else
            lid = tmp;
    }
    return lid;
}

oid_t LayerDao::parentImpl( oid_t lid )
{
    if( lid == Objects::InvalidOID )
        return Objects::InvalidOID;

    auto childType = m_g->FindType(EdgeType::CHILD_OF);
    // ID is CHILD_OF -> next ID
    std::unique_ptr<Objects> obj(m_g->Neighbors(lid, childType, Outgoing));

    if( obj->Count() == 0 )
        return Objects::InvalidOID;
    else
        return obj->Any();
}

oid_t LayerDao::childImpl( oid_t lid )
{
    if( lid == Objects::InvalidOID ) {
        return Objects::InvalidOID;
    }
    auto childType = m_g->FindType(EdgeType::CHILD_OF);
    // NEXT CHILD is CHILD_OF -> ID
    std::unique_ptr<Objects> obj(m_g->Neighbors(lid, childType, Ingoing));

    if( obj->Count() == 0 )
        return Objects::InvalidOID;
    else
        return obj->Any();
}

bool LayerDao::exists( const Layer& layer )
{
    try {
        auto type = m_g->GetObjectType(layer.id());
        if( type == Type::InvalidType )
            return false;
    } catch( Error& ) {
        return false;
    }
    return true;
}















