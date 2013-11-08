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

#include "mld/dao/MLGDao.h"
#include "mld/dao/LayerDao.h"
#include "mld/dao/SNodeDao.h"
#include "mld/dao/LinkDao.h"

using namespace mld;

MLGDao::MLGDao( dex::gdb::Graph* g )
    : AbstractDao(g)
    , m_sn( new SNodeDao(g) )
    , m_layer( new LayerDao(g) )
    , m_link( new LinkDao(g) )
    , m_ownsType(m_g->FindType(EdgeType::OWNS))
{
}

MLGDao::~MLGDao()
{
}

SuperNode MLGDao::addNodeToLayer( const Layer& l )
{
#ifdef MLD_SAFE
    bool exists = m_layer->exists(l);
    if( !exists ) {
        LOG(logERROR) << "MLGDao::addNodeToLayer: Layer doesn't exist!";
        return SuperNode();
    }
#endif
    SuperNode res = m_sn->addNode();
    // New edge OWNS: Layer -> SuperNode
    m_g->NewEdge(m_ownsType, l.id(), res.id());
    return res;
}

HLink MLGDao::addHLink( const SuperNode& src, const SuperNode& tgt, double weight )
{
#ifdef MLD_SAFE
    auto srcLayer = getLayerIdForSuperNode(src.id());
    auto tgtLayer = getLayerIdForSuperNode(tgt.id());
    if( srcLayer != tgtLayer
            || srcLayer == dex::gdb::Objects::InvalidOID
            || tgtLayer == dex::gdb::Objects::InvalidOID ) {
        LOG(logERROR) << "MLGDao::addHLink: SuperNode are not in the same layer";
        return HLink();
    }
#endif
    if( weight == kHLINK_DEF_VALUE )
        return m_link->addHLink(src.id(), tgt.id());
    else
        return m_link->addHLink(src.id(), tgt.id(), weight);
}

VLink MLGDao::addVLink( const SuperNode& src, const SuperNode& tgt, double weight )
{
#ifdef MLD_SAFE
    bool affiliated = m_layer->affiliated(src.id(), tgt.id());
    if( !affiliated ) {
        LOG(logERROR) << "MLGDao::addVLink: Layers are not affiliated";
        return VLink();
    }
#endif
    if( weight == kVLINK_DEF_VALUE )
        return m_link->addVLink(src.id(), tgt.id());
    else
        return m_link->addVLink(src.id(), tgt.id(), weight);
}

ObjectsPtr MLGDao::getAllNodeIds( const Layer& l )
{
    ObjectsPtr res;
#ifdef MLD_SAFE
    try {
#endif
        res.reset(m_g->Neighbors(l.id(), m_ownsType, dex::gdb::Outgoing));
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "MLGDao::getAllSuperNodes: " << e.Message();
    }
#endif
    return res;
}

ObjectsPtr MLGDao::getAllHLinkIds( const Layer& l )
{
    ObjectsPtr res;
#ifdef MLD_SAFE
    try {
#endif
        ObjectsPtr nodes(getAllNodeIds(l));
        res.reset(m_g->Explode(nodes.get(), m_link->hlinkType(), dex::gdb::Outgoing));
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& e ) {
        LOG(logERROR) << "MLGDao::getAllHLinks: " << e.Message();
    }
#endif
    return res;
}

std::vector<SuperNode> MLGDao::getAllSuperNode( const Layer& l )
{
    ObjectsPtr nodes(getAllNodeIds(l));
    if( !nodes )
        return std::vector<SuperNode>();
    return m_sn->getNode(nodes);
}

std::vector<HLink> MLGDao::getAllHLinks( const Layer& l )
{
    ObjectsPtr links(getAllHLinkIds(l));
    if( !links )
        return std::vector<HLink>();
    return m_link->getHLink(links);
}

// ****** PRIVATE METHODS ****** //

dex::gdb::oid_t MLGDao::getLayerIdForSuperNode( dex::gdb::oid_t nid )
{
    std::unique_ptr<dex::gdb::Objects> obj;
#ifdef MLD_SAFE
    try {
#endif
        obj.reset(m_g->Neighbors(nid, m_ownsType, dex::gdb::Ingoing));
#ifdef MLD_SAFE
    } catch( dex::gdb::Error& ) {
        LOG(logERROR) << "MLGDao::getLayerForSuperNode: invalid supernode";
        return dex::gdb::Objects::InvalidOID;
    }

    auto nb = obj->Count();
    if( nb == 0 ) {
        LOG(logERROR) << "MLGDao::getLayerForSuperNode: No Layer for node: " << nid;
        return dex::gdb::Objects::InvalidOID;
    }
    else if( nb > 1 ) {
        LOG(logERROR) << "MLGDao::getLayerForSuperNode: More than 1 layer for node: " << nid;
        return dex::gdb::Objects::InvalidOID;
    }
#endif
    return obj->Any();
}

// ****** FORWARD METHOD OF LINK DAO ****** //

HLink MLGDao::getHLink( const SuperNode& src, const SuperNode& tgt )
{
    return m_link->getHLink(src.id(), tgt.id());
}

HLink MLGDao::getHLink( dex::gdb::oid_t hid )
{
    return m_link->getHLink(hid);
}

VLink MLGDao::getVLink( const SuperNode& src, const SuperNode& tgt )
{
    return m_link->getVLink(src.id(), tgt.id());
}

VLink MLGDao::getVLink( dex::gdb::oid_t vid )
{
    return m_link->getVLink(vid);
}


// ****** FORWARD METHOD OF LAYER DAO ****** //

Layer MLGDao::addBaseLayer()
{
    return m_layer->addBaseLayer();
}

void MLGDao::setAsBaseLayer( Layer& layer )
{
    m_layer->setAsBaseLayer(layer);
}

Layer MLGDao::addLayerOnTop()
{
    return m_layer->addLayerOnTop();
}

Layer MLGDao::addLayerOnBottom()
{
    return m_layer->addLayerOnBottom();
}

bool MLGDao::removeTopLayer()
{
    return m_layer->removeTopLayer();
}

bool MLGDao::removeBottomLayer()
{
    return m_layer->removeBottomLayer();
}

bool MLGDao::removeBaseLayer()
{
    return m_layer->removeBaseLayer();
}

bool MLGDao::removeAllButBaseLayer()
{
    return m_layer->removeAllButBaseLayer();
}

Layer MLGDao::bottomLayer()
{
    return m_layer->bottomLayer();
}

Layer MLGDao::topLayer()
{
    return m_layer->topLayer();
}

Layer MLGDao::baseLayer()
{
    return m_layer->baseLayer();
}

Layer MLGDao::parent( const Layer& layer )
{
    return m_layer->parent(layer);
}

Layer MLGDao::child( const Layer& layer )
{
    return m_layer->child(layer);
}

int64_t MLGDao::countLayers()
{
    return m_layer->countLayers();
}

bool MLGDao::updateLayer( const Layer& layer )
{
    return m_layer->updateLayer(layer);
}

Layer MLGDao::getLayer( dex::gdb::oid_t id )
{
    return m_layer->getLayer(id);
}

bool MLGDao::exists( const Layer& layer )
{
    return m_layer->exists(layer);
}


