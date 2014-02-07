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
#include <dex/gdb/Values.h>
#include <dex/gdb/ValuesIterator.h>

#include "mld/Graph_types.h"

#include "mld/dao/MLGDao.h"
#include "mld/dao/LayerDao.h"
#include "mld/dao/SNodeDao.h"
#include "mld/dao/LinkDao.h"

using namespace mld;
using namespace dex::gdb;

MLGDao::MLGDao( Graph* g )
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
            || srcLayer == Objects::InvalidOID
            || tgtLayer == Objects::InvalidOID ) {
        LOG(logERROR) << "MLGDao::addHLink: SuperNodes are not on the same layer";
        return HLink();
    }
#endif
    if( weight == kHLINK_DEF_VALUE )
        return m_link->addHLink(src.id(), tgt.id());
    else
        return m_link->addHLink(src.id(), tgt.id(), weight);
}

VLink MLGDao::addVLink( const SuperNode& child, const SuperNode& parent, double weight )
{
#ifdef MLD_SAFE
    auto srcLayer = getLayerIdForSuperNode(child.id());
    auto tgtLayer = getLayerIdForSuperNode(parent.id());
    bool affiliated = m_layer->affiliated(srcLayer, tgtLayer);
    if( !affiliated ) {
        LOG(logERROR) << "MLGDao::addVLink: Layers are not affiliated";
        return VLink();
    }
#endif
    if( weight == kVLINK_DEF_VALUE )
        return m_link->addVLink(child.id(), parent.id());
    else
        return m_link->addVLink(child.id(), parent.id(), weight);
}

ObjectsPtr MLGDao::getAllNodeIds( const Layer& l )
{
    ObjectsPtr res;
#ifdef MLD_SAFE
    try {
#endif
        res.reset(m_g->Neighbors(l.id(), m_ownsType, Outgoing));
#ifdef MLD_SAFE
    } catch( Error& e ) {
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
        res.reset(m_g->Explode(nodes.get(), m_link->hlinkType(), Outgoing));
#ifdef MLD_SAFE
    } catch( Error& e ) {
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

Layer MLGDao::mirrorTopLayer()
{
    return mirrorLayerImpl(TOP);
}

Layer MLGDao::mirrorBottomLayer()
{
    return mirrorLayerImpl(BOTTOM);
}

std::vector<SuperNode> MLGDao::getParentNodes( oid_t id )
{
    std::vector<SuperNode> res;
#ifdef MLD_SAFE
    try {
#endif
        ObjectsPtr parents(m_g->Neighbors(id, m_link->vlinkType(), Outgoing));
        ObjectsIt parIt(parents->Iterator());

        while( parIt->HasNext() ) {
            res.push_back(m_sn->getNode(parIt->Next()));
        }
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "MLGDao::getParentNodes: " << e.Message();
    }
#endif
    return res;
}

std::vector<SuperNode> MLGDao::getChildNodes( oid_t id )
{
    std::vector<SuperNode> res;
#ifdef MLD_SAFE
    try {
#endif
        ObjectsPtr children(m_g->Neighbors(id, m_link->vlinkType(), Ingoing));
        ObjectsIt childIt(children->Iterator());

        while( childIt->HasNext() ) {
            res.push_back(m_sn->getNode(childIt->Next()));
        }
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "MLGDao::getChildNodes: " << e.Message();
    }
#endif
    return res;
}

int64_t MLGDao::getNodeCount( const Layer& l )
{
    ObjectsPtr nodes(getAllNodeIds(l));
    if( !nodes )
        return kINVALID_NODE_COUNT;
    return nodes->Count();
}

HLink MLGDao::getHeaviestHLink( const Layer& l )
{
    ObjectsPtr hlinkIds(getAllHLinkIds(l));
    if( !hlinkIds )
        return HLink();

    auto attr = m_g->FindAttribute(m_link->hlinkType(), H_LinkAttr::WEIGHT);
    std::unique_ptr<Values> val(m_g->GetValues(attr));
    std::unique_ptr<ValuesIterator> valIt(val->Iterator(Descendent));

    // For each value sorted by weight descendent
    while( valIt->HasNext() ) {
        // Get all hlinks for each different weight value
        ObjectsPtr oids(m_g->Select(attr, Equal, *valIt->Next()));
        // Intersection with all input layer hlinks
        ObjectsPtr res(Objects::CombineIntersection(hlinkIds.get(), oids.get()));
        // If there is at least one result
        if( res->Count() > 0 )
            return m_link->getHLink(res->Any());
    }
    return HLink();
}

HLink MLGDao::getUnsafeHeaviestHLink()
{
    auto attr = m_g->FindAttribute(m_link->hlinkType(), H_LinkAttr::WEIGHT);
    std::unique_ptr<AttributeStatistics> stats(m_g->GetAttributeStatistics(attr, true));
    // Get maximum value for H_LINK weight
    m_v->SetDouble(stats->GetMax().GetDouble());
    // Get associated HLink oids
    ObjectsPtr hlinks(m_g->Select(attr, Equal, *m_v));
    if( hlinks->Count() == 0 ) {
        LOG(logERROR) << "MLGDao::getHeaviestHLink no objects with max value";
        return HLink();
    }

    // Iterate over max values
    ObjectsIt hIt(hlinks->Iterator());
    oid_t hid = Objects::InvalidOID;

    // Iterator orders by oid, the highest oid is on the latest layer, no checks
    // Go to latest oid
    while( hIt->HasNext() ) {
        hid = hIt->Next();
    }

    return m_link->getHLink(hid);
}


// ****** PRIVATE METHODS ****** //


oid_t MLGDao::getLayerIdForSuperNode( oid_t nid )
{
    std::unique_ptr<Objects> obj;
#ifdef MLD_SAFE
    try {
#endif
        obj.reset(m_g->Neighbors(nid, m_ownsType, Ingoing));
#ifdef MLD_SAFE
    } catch( Error& ) {
        LOG(logERROR) << "MLGDao::getLayerForSuperNode: invalid supernode";
        return Objects::InvalidOID;
    }

    auto nb = obj->Count();
    if( nb == 0 ) {
        LOG(logERROR) << "MLGDao::getLayerForSuperNode: No Layer for node: " << nid;
        return Objects::InvalidOID;
    }
    else if( nb > 1 ) {
        LOG(logERROR) << "MLGDao::getLayerForSuperNode: More than 1 layer for node: " << nid;
        return Objects::InvalidOID;
    }
#endif
    return obj->Any();
}

Layer MLGDao::mirrorLayerImpl( Direction dir )
{
    Layer previous = dir == TOP ? topLayer(): bottomLayer();
#ifdef MLD_SAFE
    if( previous.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "MLGDao::mirrorTopLayerImpl: invalid previous layer";
        return previous; // invalid
    }
#endif
    // New top layer
    Layer newLayer = dir == TOP ? addLayerOnTop(): addLayerOnBottom();
    // Get all edges from previous layer
    ObjectsPtr edges(getAllHLinkIds(previous));
    ObjectsIt edgesIt(edges->Iterator());

    if( !edgesIt->HasNext() ) {
        LOG(logWARNING) << "MLGDao::mirrorTopLayerImpl: no edges in empty previous top layer" \
                           "this layer will be empty";
        return newLayer;
    }

    // Id map from previous node ids to top layer nodes, needed to add
    // corresponding edges
    NodeMap nodeMap;
    while( edgesIt->HasNext() ) {
        // Get previous layer HLink
        HLink link = m_link->getHLink(edgesIt->Next());
        mirrorEdge(link, dir, newLayer, nodeMap);
    }
    return newLayer;
}

HLink MLGDao::mirrorEdge(const HLink& current, Direction dir, const Layer& newLayer, NodeMap& nodeMap )
{
    // Find equivalent in top layer
    SuperNode topSrc;
    auto it = nodeMap.find(current.source());
    if( it != nodeMap.end() ) { // not found
        topSrc = it->second;
    }
    else {
        topSrc = mirrorNode(current.source(), dir, newLayer);
        nodeMap[current.source()] = topSrc;
    }

    SuperNode topTgt;
    it = nodeMap.find(current.target());
    if( it != nodeMap.end() ) { // not found
        topTgt = it->second;
    }
    else {
        topTgt = mirrorNode(current.target(), dir, newLayer);
        // Add to map to retrieve edges
        nodeMap[current.target()] = topTgt;
    }
    // Add new HLink
    return m_link->addHLink(topSrc.id(), topTgt.id(), current.weight());
}

SuperNode MLGDao::mirrorNode( oid_t current, Direction dir, const Layer& newLayer )
{
    // Get each node on the previous layer
    SuperNode prevNode = m_sn->getNode(current);
    // Add node in top layer
    SuperNode newNode = addNodeToLayer(newLayer);

    // Link them
    if( dir == TOP )
        m_link->addVLink(prevNode.id(), newNode.id());
    else
        m_link->addVLink(newNode.id(), prevNode.id());

    // Update node weight if needed
    if( prevNode.weight() != kSUPERNODE_DEF_VALUE ) {
        newNode.setWeight(prevNode.weight());
        m_sn->updateNode(newNode);
    }
    return newNode;
}

// ****** FORWARD METHOD OF SN DAO ****** //

void MLGDao::removeNode( oid_t id )
{
    m_sn->removeNode(id);
}

void MLGDao::updateNode( const SuperNode& n )
{
    m_sn->updateNode(n);
}

SuperNode MLGDao::getNode( oid_t id )
{
    return m_sn->getNode(id);
}

std::vector<SuperNode> MLGDao::getNode( const ObjectsPtr& objs )
{
    return m_sn->getNode(objs);
}

// ****** FORWARD METHOD OF LINK DAO ****** //

HLink MLGDao::getHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
{
    return m_link->getHLink(src, tgt);
}

HLink MLGDao::getHLink( oid_t hid )
{
    return m_link->getHLink(hid);
}

bool MLGDao::updateHLink( dex::gdb::oid_t hid, double weight )
{
    return m_link->updateHLink(hid, weight);
}

VLink MLGDao::getVLink( const SuperNode& src, const SuperNode& tgt )
{
    return m_link->getVLink(src.id(), tgt.id());
}

VLink MLGDao::getVLink( oid_t vid )
{
    return m_link->getVLink(vid);
}

std::vector<HLink> MLGDao::getHLink( const ObjectsPtr& objs )
{
    return m_link->getHLink(objs);
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

Layer MLGDao::getLayer( oid_t id )
{
    return m_layer->getLayer(id);
}

bool MLGDao::exists( const Layer& layer )
{
    return m_layer->exists(layer);
}

type_t MLGDao::hlinkType() const
{
    return m_link->hlinkType();
}

type_t MLGDao::vlinkType() const
{
    return m_link->vlinkType();
}

type_t MLGDao::superNodeType() const
{
    return m_sn->superNodeType();
}
