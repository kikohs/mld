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
#include <sparksee/gdb/Values.h>
#include <sparksee/gdb/ValuesIterator.h>

#include "mld/GraphTypes.h"
#include "mld/dao/MLGDao.h"
#include "mld/dao/LayerDao.h"
#include "mld/dao/NodeDao.h"
#include "mld/dao/LinkDao.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;

MLGDao::MLGDao( Graph* g )
    : AbstractDao(g)
    , m_node( new NodeDao(g) )
    , m_layer( new LayerDao(g) )
    , m_link( new LinkDao(g) )
{
}

MLGDao::~MLGDao()
{
}

void MLGDao::setGraph( Graph* g )
{
    assert(g != nullptr);
    AbstractDao::setGraph(g);
    m_node->setGraph(g);
    m_layer->setGraph(g);
    m_link->setGraph(g);
}

ObjectsPtr MLGDao::newObjectsPtr() const
{
    // Create a dummy request to obtain an Objects object
    auto type = m_g->FindType(NodeType::LAYER);
    ObjectsPtr ptr(m_g->Select(type));
    ptr->Clear();
    return ptr;
}

mld::Node MLGDao::addNodeToLayer( const Layer& l )
{
#ifdef MLD_SAFE
    if( !m_layer->exists(l) ) {
        LOG(logERROR) << "MLGDao::addNodeToLayer: Layer doesn't exist!";
        return Node();
    }
#endif
    Node res(m_node->addNode());
    // New edge OWNS: Layer -> Node
    m_link->addOLink(l.id(), res.id());
    return res;
}

mld::Node MLGDao::addNodeToLayer( const Layer& l, AttrMap& nodeData )
{
#ifdef MLD_SAFE
    if( !m_layer->exists(l) ) {
        LOG(logERROR) << "MLGDao::addNodeToLayer: Layer doesn't exist!";
        return Node();
    }
#endif
    Node res(m_node->addNode(nodeData));
    // New edge OWNS: Layer -> Node
    m_link->addOLink(l.id(), res.id());
    return res;
}

mld::Node MLGDao::addNodeToLayer( const Layer& l, AttrMap& nodeData, AttrMap& oLinkData )
{
#ifdef MLD_SAFE
    if( !m_layer->exists(l) ) {
        LOG(logERROR) << "MLGDao::addNodeToLayer: Layer doesn't exist!";
        return Node();
    }
#endif
    Node res(m_node->addNode(nodeData));
    // New edge OWNS: Layer -> Node
    m_link->addOLink(l.id(), res.id(), oLinkData);
    return res;
}

HLink MLGDao::addHLink( const Node& src, const Node& tgt, double weight )
{
//#ifdef MLD_SAFE
//    auto srcLayer = getLayerIdForNode(src.id());
//    auto tgtLayer = getLayerIdForNode(tgt.id());
//    if( srcLayer != tgtLayer
//            || srcLayer == Objects::InvalidOID
//            || tgtLayer == Objects::InvalidOID ) {
//        LOG(logERROR) << "MLGDao::addHLink: Nodes are not on the same layer";
//        return HLink();
//    }
//#endif
    if( weight == HLINK_DEF_VALUE )
        return m_link->addHLink(src.id(), tgt.id());
    else
        return m_link->addHLink(src.id(), tgt.id(), weight);
}

HLink MLGDao::addHLink( const Node& src, const Node& tgt, AttrMap& data )
{
//#ifdef MLD_SAFE
//    auto srcLayer = getLayerIdForNode(src.id());
//    auto tgtLayer = getLayerIdForNode(tgt.id());
//    if( srcLayer != tgtLayer
//            || srcLayer == Objects::InvalidOID
//            || tgtLayer == Objects::InvalidOID ) {
//        LOG(logERROR) << "MLGDao::addHLink: Nodes are not on the same layer";
//        return HLink();
//    }
//#endif
    return m_link->addHLink(src.id(), tgt.id(), data);
}

VLink MLGDao::addVLink( const Node& child, const Node& parent, double weight )
{
#ifdef MLD_SAFE
    auto srcLayer = getLayerIdForNode(child.id());
    auto tgtLayer = getLayerIdForNode(parent.id());
    bool affiliated = m_layer->affiliated(srcLayer, tgtLayer);
    if( !affiliated ) {
        LOG(logERROR) << "MLGDao::addVLink: Layers are not affiliated";
        return VLink();
    }
#endif
    if( weight == VLINK_DEF_VALUE )
        return m_link->addVLink(child.id(), parent.id());
    else
        return m_link->addVLink(child.id(), parent.id(), weight);
}

VLink MLGDao::addVLink( const Node& child, const Node& parent, AttrMap& data )
{
#ifdef MLD_SAFE
    auto srcLayer = getLayerIdForNode(child.id());
    auto tgtLayer = getLayerIdForNode(parent.id());
    bool affiliated = m_layer->affiliated(srcLayer, tgtLayer);
    if( !affiliated ) {
        LOG(logERROR) << "MLGDao::addVLink: Layers are not affiliated";
        return VLink();
    }
#endif
    return m_link->addVLink(child.id(), parent.id(), data);
}

OLink MLGDao::addOLink( const Layer& layer, const Node& node, AttrMap& data )
{
#ifdef MLD_SAFE
    if( !m_layer->exists(layer) ) {
        LOG(logERROR) << "MLGDao::addOLink: Layer doesn't exist!";
        return OLink();
    }
#endif
    return m_link->addOLink(layer.id(), node.id(), data);
}

ObjectsPtr MLGDao::getAllNodeIds( const Layer& l )
{
    ObjectsPtr res;
#ifdef MLD_SAFE
    try {
#endif
        res.reset(m_g->Neighbors(l.id(), m_link->olinkType(), Outgoing));
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "MLGDao::getAllNodes: " << e.Message();
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

NodeVec MLGDao::getAllNodes( const Layer& l )
{
    ObjectsPtr nodes(getAllNodeIds(l));
    if( !nodes )
        return NodeVec();
    return m_node->getNode(nodes);
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

std::vector<mld::Node> MLGDao::getParentNodes( oid_t id )
{
    return m_node->getNode(getParentIds(id));
}

ObjectsPtr MLGDao::getParentIds( oid_t id )
{
    return getVLinkEndpoints(id, TOP);
}

NodeVec MLGDao::getChildNodes( oid_t id )
{
    return m_node->getNode(getChildIds(id));
}

ObjectsPtr MLGDao::getChildIds( oid_t id )
{
    return getVLinkEndpoints(id, BOTTOM);
}

bool MLGDao::checkAffiliation( sparksee::gdb::oid_t source, sparksee::gdb::oid_t target, Direction dir )
{
    ObjectsPtr p(getVLinkEndpoints(source, dir));
    if( !p->Exists(target) ) {
        LOG(logERROR) << " MLGDao::checkAffiliation: source and target node are not affiliated";
        return false;
    }
    return true;
}

int64_t MLGDao::getNodeCount( const Layer& l )
{
    ObjectsPtr nodes(getAllNodeIds(l));
    if( !nodes )
        return INVALID_NODE_COUNT;
    return nodes->Count();
}

int64_t MLGDao::getHLinkCount( const Layer& l )
{
    ObjectsPtr edges(getAllHLinkIds(l));
    if( !edges )
        return INVALID_EDGE_COUNT;
    return edges->Count();
}

HLink MLGDao::getHeaviestHLink( const Layer& l )
{
    ObjectsPtr hlinkIds(getAllHLinkIds(l));
    if( !hlinkIds )
        return HLink();

    auto attr = m_g->FindAttribute(m_link->hlinkType(), Attrs::V[HLinkAttr::WEIGHT]);
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
    auto attr = m_g->FindAttribute(m_link->hlinkType(), Attrs::V[HLinkAttr::WEIGHT]);
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

bool MLGDao::horizontalCopyVLinks( const Node& source,
                                   const Node& target,
                                   bool safe,
                                   const ObjectsPtr& subset,
                                   const WeightMergerFunc& f )
{
    return horizontalCopyLinks(vlinkType(), source, target, safe, subset, f);
}

bool MLGDao::horizontalCopyHLinks( const Node& source,
                                   const Node& target,
                                   bool safe,
                                   const ObjectsPtr& subset,
                                   const WeightMergerFunc& f )
{
    return horizontalCopyLinks(hlinkType(), source, target, safe, subset, f);
}

bool MLGDao::verticalCopyHLinks( const Node& source,
                                 const Node& target,
                                 Direction dir,
                                 bool safe,
                                 const ObjectsPtr& subset,
                                 const WeightMergerFunc& f )
{
    if( safe ) {  // Check source and target affiliation
        if( !checkAffiliation(source.id(), target.id(), dir) ) {
            return false;
        }
    }

    // Get source's neighbors
    ObjectsPtr srcNeighbors;
    if( subset )
        srcNeighbors = subset; // Temporary borrow
    else
        srcNeighbors.reset(m_g->Neighbors(source.id(), hlinkType(), Any));

    ObjectsIt it(srcNeighbors->Iterator());
    while( it->HasNext() ) {
        oid_t current = it->Next();
        NodeVec kin;
        if( dir == TOP )
            kin = getParentNodes(current);
        else
            kin = getChildNodes(current);

        if( kin.empty() ) {
            LOG(logERROR) << "MLGDao::verticalCopyHLinks: node as no parents";
            return false;
        }

        HLink currentLink = getHLink(source.id(), current);
        // For each child or parent
        for( Node& k: kin ) {
            HLink link = getHLink(target.id(), k.id());
            if( link.id() == Objects::InvalidOID ) {  // Top HLink doesn't exist
                addHLink(target, k, currentLink.weight());
            }
            else {  // Update top HLink with functor
                link.setWeight( f(link.weight(), currentLink.weight()) );
                if( !updateHLink(link) ) {
                    LOG(logERROR) << "MLGDao::verticalCopyHLinks: Failed to update top/bottom nodes HLINK";
                    return false;
                }
            }
        }
    }
    return true;
}

// ****** PRIVATE METHODS ****** //


oid_t MLGDao::getLayerIdForNode( oid_t nid )
{
    ObjectsPtr obj;
#ifdef MLD_SAFE
    try {
#endif
        obj.reset(m_g->Neighbors(nid, m_link->olinkType(), Ingoing));
#ifdef MLD_SAFE
    } catch( Error& ) {
        LOG(logERROR) << "MLGDao::getLayerForNode: invalid supernode";
        return Objects::InvalidOID;
    }

    auto nb = obj->Count();
    if( nb == 0 ) {
        LOG(logERROR) << "MLGDao::getLayerForNode: No Layer for node: " << nid;
        return Objects::InvalidOID;
    }
    else if( nb > 1 ) {
        LOG(logERROR) << "MLGDao::getLayerForNode: More than 1 layer for node: " << nid;
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

    LOG(logINFO) << "Mirroring layer";
    ProgressDisplay display(edges->Count());

    // Id map from previous node ids to top layer nodes, needed to add
    // corresponding edges
    NodeMap nodeMap;
    while( edgesIt->HasNext() ) {
        // Get previous layer HLink
        HLink link = m_link->getHLink(edgesIt->Next());
        mirrorEdge(link, dir, newLayer, nodeMap);
        ++display;
    }
    return newLayer;
}

HLink MLGDao::mirrorEdge( const HLink& current, Direction dir, const Layer& newLayer, NodeMap& nodeMap )
{
    // Find equivalent in top layer
    Node topSrc;
    auto it = nodeMap.find(current.source());
    if( it != nodeMap.end() ) { // not found
        topSrc = it->second;
    }
    else {
        topSrc = mirrorNode(current.source(), dir, newLayer);
        nodeMap.emplace(current.source(), topSrc);
    }

    Node topTgt;
    it = nodeMap.find(current.target());
    if( it != nodeMap.end() ) { // not found
        topTgt = it->second;
    }
    else {
        topTgt = mirrorNode(current.target(), dir, newLayer);
        // Add to map to retrieve edges
        nodeMap.emplace(current.target(), topTgt);
    }
    // Add new HLink
    return m_link->addHLink(topSrc.id(), topTgt.id(), current.weight());
}

mld::Node MLGDao::mirrorNode( oid_t current, Direction dir, const Layer& newLayer )
{
    // Get each node on the previous layer
    Node prevNode = m_node->getNode(current);
    // Add node in top layer
    Node newNode = addNodeToLayer(newLayer);

    // Link them
    if( dir == TOP )
        m_link->addVLink(prevNode.id(), newNode.id());
    else
        m_link->addVLink(newNode.id(), prevNode.id());

    // Update node weight if needed
    if( prevNode.weight() != NODE_DEF_VALUE ) {
        newNode.setWeight(prevNode.weight());
        m_node->updateNode(newNode);
    }
    return newNode;
}

ObjectsPtr MLGDao::getVLinkEndpoints( oid_t current, Direction dir )
{
    ObjectsPtr res;
#ifdef MLD_SAFE
    try {
#endif
        if( dir == TOP )
            res.reset(m_g->Neighbors(current, m_link->vlinkType(), Outgoing));
        else
            res.reset(m_g->Neighbors(current, m_link->vlinkType(), Ingoing));
#ifdef MLD_SAFE
    } catch( Error& e ) {
        LOG(logERROR) << "MLGDao::getVLinkEndpoints: " << e.Message();
    }
#endif
    return res;
}

bool MLGDao::horizontalCopyLinks( type_t linkType,
                                const Node& source,
                                const Node& target,
                                bool safe,
                                const ObjectsPtr& subset,
                                const WeightMergerFunc& f )
{
    if( safe ) {
        auto srcLayer = getLayerIdForNode(source.id());
        auto tgtLayer = getLayerIdForNode(target.id());
        if( srcLayer != tgtLayer ) {
            LOG(logERROR) << "MLGDao::copyAndMergeLinks: Nodes are not on the same layer";
            return false;
        }
    }
    // Get target's neighbors
    ObjectsPtr tgtNeighbors(m_g->Neighbors(target.id(), linkType, Any));
    // Get source's neighbors
    ObjectsPtr srcNeighbors;
    if( subset )
        srcNeighbors = subset;
    else
        srcNeighbors.reset(m_g->Neighbors(source.id(), linkType, Any));

    // Get common neighbors
    ObjectsPtr commonNeighbors( Objects::CombineIntersection(tgtNeighbors.get(), srcNeighbors.get()) );
    // If any merge edge weight on target node
    if( commonNeighbors->Count() != 0 ) {
        ObjectsIt it(commonNeighbors->Iterator());
        while( it->HasNext() ) {
            oid_t common = it->Next();

            if( linkType == hlinkType() ) {  // HLINK
                HLink tLink = getHLink(target.id(), common);
                HLink sLink = getHLink(source.id(), common);
                // Add weight and update HLink according to merger function
                tLink.setWeight(f(tLink.weight(), sLink.weight()));
                if( !updateHLink(tLink) ) {
                    LOG(logERROR) << "MLGDao:copyAndMergeHLinks update common HLink failed: " << tLink;
                    return false;
                }
            }
            else {  // VLINK
                VLink tLink = getVLink(common, target.id());
                VLink sLink;
                if( tLink.id() != Objects::InvalidOID ) {  // VLink to lower layer
                    sLink = getVLink(common, source.id());
                }
                else {  // VLink to upper layer
                    tLink = getVLink(target.id(), common);
                    sLink = getVLink(source.id(), common);
                }
                // Add weight and update HLink according to merger function
                tLink.setWeight(f(tLink.weight(), sLink.weight()));
                if( !updateVLink(tLink) ) {
                    LOG(logERROR) << "MLGDao:copyAndMergeHLinks update common VLink failed: " << tLink;
                    return false;
                }
            }
        }
    }

    // Add the rest of source's HLinks or VLinks (remove common neighbors)
    // Remove target id
    srcNeighbors->Remove(target.id());
    ObjectsPtr onlySrcNeighbors( Objects::CombineDifference(srcNeighbors.get(), commonNeighbors.get()) );
    if( onlySrcNeighbors->Count() != 0 ) {
        ObjectsIt it(onlySrcNeighbors->Iterator());

        // No self-loops allowed
        if( linkType == hlinkType() ) {
            while( it->HasNext() ) {
                oid_t n = it->Next();
                HLink sLink = getHLink(source.id(), n);
                // Create new hlink
                m_link->addHLink(target.id(), n, sLink.weight());
            }
        }
        else {  // VLink
            while( it->HasNext() ) {
                oid_t n = it->Next();
                VLink sLink = getVLink(n, source.id());
                if( sLink.id() != Objects::InvalidOID ) { // VLink to lower layer
                    m_link->addVLink(n, target.id(), sLink.weight());
                }
                else { // VLink to upper layer
                    sLink = getVLink(source.id(), n);
                    if( sLink.id() != Objects::InvalidOID ) {
                        m_link->addVLink(target.id(), n, sLink.weight());
                    }
                    else {
                        LOG(logERROR) << "MLGDao::copyAndMergeLinks VLink from " << source << " to "
                                         << n << " is neither an upper or lower VLink";
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

std::vector<OLink> MLGDao::getAllOLinks( oid_t nodeId )
{
    std::vector<OLink> res;
    auto layers(getAllLayers()); // Get layers ordered bot to top
    for( auto& l : layers ) {
        res.push_back(getOLink(l.id(), nodeId));
    }
    return res;
}

TimeSeries<double> MLGDao::getSignal( oid_t nodeId, oid_t bottomLayer, oid_t topLayer )
{
#ifdef MLD_SAFE
    if( bottomLayer == Objects::InvalidOID
        || topLayer == Objects::InvalidOID
        || nodeId == Objects::InvalidOID ) {

        LOG(logERROR) << "MLGDao::getSignal: invalid ids";
        return TimeSeries<double>();
    }
#endif

    // Use as less overhead as possible
    TimeSeries<double> res;
    oid_t layer = bottomLayer;
    type_t oType = m_link->olinkType();
    Value v;
    while( layer != topLayer ) {
        oid_t eid = findEdge(oType, layer, nodeId);
        m_g->GetAttribute(eid, m_g->FindAttribute(oType, Attrs::V[OLinkAttr::WEIGHT]), v);
        res.data().push_back(v.GetDouble());
        layer = m_layer->parentImpl(layer);
    }

    // Don't forget last layer, it is inclusive
    oid_t eid = findEdge(oType, layer, nodeId);
    m_g->GetAttribute(eid, m_g->FindAttribute(oType, Attrs::V[OLinkAttr::WEIGHT]), v);
    res.data().push_back(v.GetDouble());
    res.clamp();
    return res;
}

TimeSeries<double> MLGDao::getSignal( oid_t nodeId, oid_t currentLayer, TSDirection dir, size_t radius )
{
#ifdef MLD_SAFE
    if( nodeId == Objects::InvalidOID || currentLayer == Objects::InvalidOID ) {
        LOG(logERROR) << "MLGDao::getSignal invalid ids";
        return TimeSeries<double>();
    }
#endif
    oid_t bot = currentLayer;
    oid_t top = currentLayer;

    if( dir != TSDirection::FUTURE ) {  // PAST or BOTH
        bool stop = false;
        size_t level = 0;
        while( !stop && level != radius ) {
            auto tmp = m_layer->childImpl(bot);
            if( tmp == Objects::InvalidOID ) {
                stop = true;
            }
            else {
                bot = tmp;
                level++;
            }
        }
    }

    if( dir != TSDirection::PAST ) {  // FUTURE or BOTH
        bool stop = false;
        size_t level = 0;
        while( !stop && level != radius ) {
            auto tmp = m_layer->parentImpl(top);
            if( tmp == Objects::InvalidOID ) {
                stop = true;
            }
            else {
                top = tmp;
                level++;
            }
        }
    }

    auto res(getSignal(nodeId, bot, top));
    res.setRadius(radius);
    res.setDirection(dir);
    return res;
}

// ****** FORWARD METHOD OF SN DAO ****** //

void MLGDao::removeNode( oid_t id )
{
    m_node->removeNode(id);
}

bool MLGDao::updateNode( Node& n )
{
    return m_node->updateNode(n);
}

mld::Node MLGDao::getNode( oid_t id )
{
    return m_node->getNode(id);
}

NodeVec MLGDao::getNode( const ObjectsPtr& objs )
{
    return m_node->getNode(objs);
}

// ****** FORWARD METHOD OF LINK DAO ****** //

HLink MLGDao::getHLink( oid_t src, oid_t tgt )
{
    return m_link->getHLink(src, tgt);
}

HLink MLGDao::getHLink( oid_t hid )
{
    return m_link->getHLink(hid);
}

bool MLGDao::updateHLink( HLink& link )
{
    return m_link->updateHLink(link.id(), link.data());
}

VLink MLGDao::getVLink( oid_t src, oid_t tgt )
{
    return m_link->getVLink(src, tgt);
}

VLink MLGDao::getVLink( oid_t vid )
{
    return m_link->getVLink(vid);
}

std::vector<HLink> MLGDao::getHLink( const ObjectsPtr& objs )
{
    return m_link->getHLink(objs);
}

bool MLGDao::updateVLink( VLink& link )
{
    return m_link->updateVLink(link.id(), link.data());
}

OLink MLGDao::getOLink( oid_t layerId, oid_t nodeId )
{
    return m_link->getOLink(layerId, nodeId);
}

OLink MLGDao::getOLink( oid_t eid )
{
    return m_link->getOLink(eid);
}

bool MLGDao::updateOLink( OLink& link )
{
    return m_link->updateOLink(link.id(), link.data());
}

void MLGDao::removeOLink( oid_t layerId, oid_t nodeId )
{
    m_link->removeOLink(layerId, nodeId);
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

int64_t MLGDao::getLayerCount()
{
    return m_layer->getLayerCount();
}

void MLGDao::updateLayer( Layer& layer )
{
    m_layer->updateLayer(layer);
}

Layer MLGDao::getLayer( oid_t id )
{
    return m_layer->getLayer(id);
}

bool MLGDao::exists( const Layer& layer )
{
    return m_layer->exists(layer);
}

std::vector<Layer> MLGDao::getAllLayers()
{
    return m_layer->getAllLayers();
}

CLink MLGDao::topCLink( sparksee::gdb::oid_t lid )
{
    return m_layer->topCLink(lid);
}

CLink MLGDao::bottomCLink( sparksee::gdb::oid_t lid )
{
    return m_layer->bottomCLink(lid);
}

CLink MLGDao::getCLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt )
{
    return m_layer->getCLink(src, tgt);
}

CLink MLGDao::getCLink( sparksee::gdb::oid_t clid )
{
    return m_layer->getCLink(clid);
}

bool MLGDao::updateCLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, double weight )
{
    return m_layer->updateCLink(child, parent, weight);
}

bool MLGDao::updateCLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, AttrMap& data )
{
    return m_layer->updateCLink(child, parent, data);
}

bool MLGDao::updateCLink( sparksee::gdb::oid_t eid, AttrMap& data )
{
    return m_layer->updateCLink(eid, data);
}

bool MLGDao::updateCLink( sparksee::gdb::oid_t eid, double weight )
{
    return m_layer->updateCLink(eid, weight);
}

// Types
type_t MLGDao::hlinkType() const
{
    return m_link->hlinkType();
}

type_t MLGDao::vlinkType() const
{
    return m_link->vlinkType();
}

type_t MLGDao::nodeType() const
{
    return m_node->nodeType();
}

type_t MLGDao::olinkType() const
{
    return m_link->olinkType();
}
