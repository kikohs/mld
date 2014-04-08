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

#include "mld/GraphTypes.h"
#include "mld/dao/LayerDao.h"

using namespace mld;
using namespace sparksee::gdb;

LayerDao::LayerDao( Graph* g )
    : AbstractDao(g)
{
    setGraph(g);
}

LayerDao::~LayerDao()
{
    // DO NOT DELETE GRAPH
}

void LayerDao::setGraph( Graph* g )
{
    if( g ) {
        AbstractDao::setGraph(g);
        m_layerType = m_g->FindType(NodeType::LAYER);
        m_clinkType = m_g->FindType(EdgeType::CHILD_OF);
        // Create a dummy layer to get the default attributes
        oid_t id = addLayer();
        oid_t id2 = addLayer();
        m_layerAttr = readAttrMap(id);
        auto clink = m_g->NewEdge(m_clinkType, id, id2);
        m_clinkAttr = readAttrMap(clink);
        m_g->Drop(id);
        m_g->Drop(id2);
    }
}

int64_t LayerDao::getLayerCount()
{
    std::unique_ptr<Objects> obj(m_g->Select(m_layerType));
    return obj->Count();
}

Layer LayerDao::addLayerOnTop()
{
    auto base = baseLayerImpl();
    if( base == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::addLayerOnTop: need a base layer";
        return Layer();
    }

    oid_t newId = addLayer();
    if( !attachOnTop(newId) ) {
        LOG(logERROR) << "LayerDao::addLayerOnTop: attach failed";
        return Layer();
    }
    return Layer(newId, m_layerAttr);
}

Layer LayerDao::addLayerOnBottom()
{
    auto base = baseLayerImpl();
    if( base == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::addLayerOnBottom: need a base layer";
        return Layer();
    }

    oid_t newId = addLayer();
    if( !attachOnBottom(newId) ) {
        LOG(logERROR) << "LayerDao::addLayerOnBottom: attach failed";
        return Layer();
    }
    return Layer(newId, m_layerAttr);
}

void LayerDao::updateLayer( Layer& layer )
{
    // Remove the key isBaseLayer not to update this value
    bool isBase = layer.isBaseLayer();
    AttrMap& data = layer.data();
    auto it = data.find(Attrs::V[LayerAttr::IS_BASE]);
    data.erase(it);
    updateAttrMap(m_layerType, layer.id(), data);
    // Add key to object
    data[Attrs::V[LayerAttr::IS_BASE]].SetBooleanVoid(isBase);
}

Layer LayerDao::getLayer( oid_t id )
{
    if( id == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::getLayer: Get on invalid Layer oid";
        return Layer();
    }

    AttrMap data;
#ifdef MLD_SAFE
    try {
#endif
        data = readAttrMap(id);
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "LayerDao::getLayer: " << e.Message();
        return Layer();
    }
#endif
    return Layer(id, data);
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
    layer.setIsBaseLayer(true);
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
    return getLayer(parent(layer.id()));
}

Layer LayerDao::child( const Layer& layer )
{
    return getLayer(child(layer.id()));
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
    auto nb = getLayerCount();
    auto base = baseLayerImpl();

    if( nb == 1 && base != Objects::InvalidOID )
        return removeLayer(base);

    return false;
}

bool LayerDao::removeAllButBaseLayer()
{
    // Remove all top layers
    while( removeTopLayer() );
    // Remove all bottom layers
    while( removeBottomLayer() );
    auto nb = getLayerCount();
    return nb == 1 ? true : false;
}

std::vector<Layer> LayerDao::getAllLayers()
{
    std::vector<Layer> res;
    Layer bot(bottomLayer());
    if( bot.id() == Objects::InvalidOID )
        return res;

    // Add bottom Layer
    res.push_back(bot);

    // Loop through all parent layers from the bottom
    oid_t pId = bot.id();
    while( pId != Objects::InvalidOID ) {
        pId = parent(pId);
        if( pId != Objects::InvalidOID )
            res.push_back(getLayer(pId));
    }
    return res;
}

bool LayerDao::affiliated( oid_t layer1, oid_t layer2 )
{
    oid_t eid = Objects::InvalidOID;
#ifdef MLD_SAFE
    try {
#endif
        // Check child_of
        eid = m_g->FindEdge(m_clinkType, layer1, layer2);
        if( eid == Objects::InvalidOID ) {
            // Check reverse child_of
            eid = m_g->FindEdge(m_clinkType, layer2, layer1);
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

CLink LayerDao::topCLink( oid_t lid )
{
    oid_t top = parent(lid);
    if( top == Objects::InvalidOID )
        return CLink();
    return getCLink(lid, top);
}

CLink LayerDao::bottomCLink( oid_t lid )
{
    oid_t c = child(lid);
    if( c == Objects::InvalidOID )
        return CLink();
    return getCLink(c, lid);
}

CLink LayerDao::getCLink( oid_t src, oid_t tgt )
{
    return getLink<CLink>(m_clinkType, src, tgt);
}

CLink LayerDao::getCLink( oid_t clid )
{
    return getLink<CLink>(m_clinkType, clid);
}

bool LayerDao::updateCLink( oid_t child, oid_t parent, double weight )
{
    AttrMap data(m_clinkAttr);
    data[Attrs::V[CLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return updateAttrMap(m_clinkType, findEdge(m_clinkType, child, parent), data);
}

bool LayerDao::updateCLink( oid_t child, oid_t parent, AttrMap& data )
{
    return updateAttrMap(m_clinkType, findEdge(m_clinkType, child, parent), data);
}

bool LayerDao::updateCLink( oid_t eid, AttrMap& data )
{
    return updateAttrMap(m_clinkType, eid, data);
}

bool LayerDao::updateCLink( oid_t eid, double weight )
{
    AttrMap data;
    data[Attrs::V[CLinkAttr::WEIGHT]].SetDoubleVoid(weight);
    return updateAttrMap(m_clinkType, eid, data);
}

// ********** PRIVATE METHODS ********** //
oid_t LayerDao::addLayer()
{
    return m_g->NewNode(m_layerType);
}

bool LayerDao::removeLayer( oid_t lid )
{
    if( lid == Objects::InvalidOID ) {
        LOG(logERROR) << "LayerDao::removeLayer: Attempting to remove an invalid layer, abort";
        return false;
    }

    auto ownsType = m_g->FindType(EdgeType::OLINK);
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

    m_g->NewEdge(m_clinkType, top, newId);
    return true;
}

bool LayerDao::attachOnBottom( oid_t newId )
{
    auto bottom = bottomLayerImpl();
    if( bottom == Objects::InvalidOID )
        return false;

    m_g->NewEdge(m_clinkType, newId, bottom);
    return true;
}

oid_t LayerDao::baseLayerImpl()
{
    auto attr = m_g->FindAttribute(m_layerType, Attrs::V[LayerAttr::IS_BASE]);

    std::unique_ptr<Objects> obj(m_g->Select(attr, Equal, m_v->SetBoolean(true)));
    if( obj->Count() == 0 ) {
        return Objects::InvalidOID;
    }
    // Should only contain 1 element
    return obj->Any();
}

void LayerDao::setAsBaseLayerImpl( oid_t newId )
{
    auto attr = m_g->FindAttribute(m_layerType, Attrs::V[LayerAttr::IS_BASE]);
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
        auto tmp = parent(lid);
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
        auto tmp = child(lid);
        if( tmp == Objects::InvalidOID )
            stop = true;
        else
            lid = tmp;
    }
    return lid;
}

oid_t LayerDao::parent( oid_t lid )
{
    if( lid == Objects::InvalidOID )
        return Objects::InvalidOID;

    // ID is CHILD_OF -> next ID
    std::unique_ptr<Objects> obj(m_g->Neighbors(lid, m_clinkType, Outgoing));

    if( obj->Count() == 0 )
        return Objects::InvalidOID;
    else
        return obj->Any();
}

oid_t LayerDao::child( oid_t lid )
{
    if( lid == Objects::InvalidOID ) {
        return Objects::InvalidOID;
    }
    // NEXT CHILD is CHILD_OF -> ID
    std::unique_ptr<Objects> obj(m_g->Neighbors(lid, m_clinkType, Ingoing));

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















