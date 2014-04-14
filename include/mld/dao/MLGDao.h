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

#ifndef MLD_MLGDAO_H
#define MLD_MLGDAO_H

#include <unordered_map>
#include <functional>

#include "mld/common.h"
#include "mld/dao/AbstractDao.h"
#include "mld/model/Node.h"
#include "mld/model/Layer.h"
#include "mld/model/Link.h"
#include "mld/model/TimeSeries.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
}}

namespace mld {
    class NodeDao;
    class LayerDao;
    class LinkDao;
}

namespace mld {

using WeightMergerFunc =  std::function<double (double, double)>;
using NodeVec = std::vector<Node>;
using LayerIdPair = std::pair<sparksee::gdb::oid_t, sparksee::gdb::oid_t>;

/**
 * @brief The MultiLayerGraph (MLG) dao
 * Main class for accessing the underlying database structure
 */
class MLD_API MLGDao: public AbstractDao
{
public:
    enum Direction {
        TOP,
        BOTTOM
    };

    using NodeMap = std::unordered_map<sparksee::gdb::oid_t, Node>;

    MLGDao( sparksee::gdb::Graph* g );
    virtual ~MLGDao() override;

    virtual void setGraph( sparksee::gdb::Graph* g ) override;
    /**
     * @brief Return new empty ObjectsPtr
     * @return
     */
    ObjectsPtr newObjectsPtr() const;

    /**
     * @brief Add a node to a specified Layer
     * @param if MLD_SAFE flag is set, it checks for the existence of the layer
     * @return
     */
    Node addNodeToLayer( const Layer& l );
    Node addNodeToLayer( const Layer& l, AttrMap& nodeData );
    Node addNodeToLayer( const Layer& l, AttrMap& nodeData, AttrMap& oLinkData );

    /**
     * @brief Add HLink
     * If MLG_SAFE flag is on, checks that the 2 nodes belong to the same layer
     * @param src Supernode source
     * @param tgt Supernode tgt
     * @param weight
     * @return New created HLink
     */
    HLink addHLink( const Node& src, const Node& tgt, double weight=HLINK_DEF_VALUE );
    HLink addHLink( const Node& src, const Node& tgt, AttrMap& data );

    /**
     * @brief Add VLink
     * If MLG_SAFE flag is on, checks that the 2 nodes are in adajacent layers
     * @param src source
     * @param tgt tgt
     * @param weight
     * @return New created VLink
     */
    VLink addVLink( const Node& child, const Node& parent, double weight=VLINK_DEF_VALUE );
    VLink addVLink( const Node& src, const Node& tgt, AttrMap& data );

    /**
     * @brief Add OLink
     * @param layer src Layer
     * @param tgttgt Node
     * @param data
     * @return newly created OLink
     */
    OLink addOLink( const Layer& layer, const Node& node, AttrMap& data );

    /**
     * @brief Get all nodes ids belonging to input layer
     * @param l Input layer
     * @return Nodes
     */
    ObjectsPtr getAllNodeIds( const Layer& l );

    /**
     * @brief Get all Hlink ids for an input layer
     * @param l Input layer
     * @return HLinks
     */
    ObjectsPtr getAllHLinkIds( const Layer& l );

    /**
     * @brief Get all nodes belonging to input layer
     * @param l Input layer
     * @return Nodes
     */
    NodeVec getAllNodes( const Layer& l );

    /**
     * @brief Get all Hlink for an input layer
     * @param l Input layer
     * @return HLinks
     */
    std::vector<HLink> getAllHLinks( const Layer& l );

    /**
     * @brief Mirror top layer
     * Duplicate top layer and link each new supernode to the
     * previous one with a VLINK
     * @return top layer
     */
    Layer mirrorTopLayer();

    /**
     * @brief Mirror bottom layer
     * Duplicate bottom layer and link each new supernode to the
     * previous one with a VLINK
     * @return bottom layer
     */
    Layer mirrorBottomLayer();

    /**
     * @brief Get parent nodes for the given supernode id
     * @param id SuperNode id
     * @return Parent SuperNodes
     */
    NodeVec getParentNodes( sparksee::gdb::oid_t id );
    ObjectsPtr getParentIds( sparksee::gdb::oid_t id );

    /**
     * @brief Get child nodes for the given supernode id
     * @param id SuperNode id
     * @return Child SuperNodes
     */
    NodeVec getChildNodes( sparksee::gdb::oid_t id );
    ObjectsPtr getChildIds( sparksee::gdb::oid_t id );

    /**
     * @brief Check if source and target node are either parent
     * or child node from each other.
     * @param source Source node id
     * @param target Target node id
     * @param dir Direction
     * @return affiliation
     */
    bool checkAffiliation( sparksee::gdb::oid_t source, sparksee::gdb::oid_t target, Direction dir );

    /**
     * @brief Get number of SuperNode for input layer
     * @param l Input Layer
     * @return count
     */
    int64_t getNodeCount( const Layer& l );

    /**
     * @brief Get number of HLinks for input layer
     * @param l Input Layer
     * @return count
     */
    int64_t getHLinkCount( const Layer& l );

    /**
     * @brief Get heaviest edge between supernodes on layer
     * @param l Input Layer
     * @return heaviest HLink
     */
    HLink getHeaviestHLink( const Layer& l );

    /**
     * @brief Get heaviest edge between supernodes
     * it assumes that the heaviest weight is always on the latest layer with the highest oid values.
     * Skip validation and does not need a layer in input.
     * @return heaviest HLink
     */
    HLink getUnsafeHeaviestHLink();

    /**
     * @brief Copy all Vlinks from source to target, applying merge function f
     * for common VLinks. Nodes have to be on the same layer.
     * @param source
     * @param target
     * @param safe Check if nodes are on the same layer before merging
     * @param f Merge function for common links
     * @return success
     */
    bool horizontalCopyVLinks( const Node& source,
                               const Node& target,
                               bool safe=true,
                               const ObjectsPtr& subset=ObjectsPtr(),
                               const WeightMergerFunc& f=[](double a, double b) {return a + b;} );

    /**
     * @brief Copy all HLinks form source node to target, applying merge function f
     * for common HLinks. Nodes have to be on the same layer.
     * If the target
     * @param source
     * @param target
     * @param safe Check if nodes are on the same layer before merging
     * @param f Merge function for common links
     * @return success
     */
    bool horizontalCopyHLinks( const Node& source,
                               const Node& target,
                               bool safe=true,
                               const ObjectsPtr& subset=ObjectsPtr(),
                               const WeightMergerFunc& f=[](double a, double b) {return a + b;} );

    /**
     * Vertical copy of all HLinks from source node on Layer k to target node on
     * Layer k+1 or k-1
     * @param source Source Node
     * @param target Target Node
     * @param d Direction TOP or BOTTOM
     * @param subset Subset of HLinks to copy, if empty, all HLinks are copied
     * @param safe Check if nodes are on the same layer before merging
     * @param f Merge function for common links
     * @return success
     */
    bool verticalCopyHLinks( const Node& source,
                             const Node& target,
                             Direction direction=TOP,
                             bool safe=true,
                             const ObjectsPtr& subset=ObjectsPtr(),
                             const WeightMergerFunc& f=[](double a, double b) {return a + b;} );

    inline Node invalidNode() const { return Node(); }

    /**
     * @brief Get Olinks
     * @param nodeId
     * @return
     */
    std::vector<OLink> getAllOLinks( sparksee::gdb::oid_t nodeId );

    TimeSeries<double> getSignal( sparksee::gdb::oid_t nodeId,
                                  sparksee::gdb::oid_t bottomLayer,
                                  sparksee::gdb::oid_t topLayer );

    TimeSeries<double> getSignal( sparksee::gdb::oid_t nodeId,
                                  sparksee::gdb::oid_t currentLayer,
                                  TSDirection dir, size_t radius );

    LayerIdPair getLayerBounds( sparksee::gdb::oid_t source, TSDirection dir, size_t radius );

    // Forward to SNDao
    void removeNode( sparksee::gdb::oid_t id );
    bool updateNode( Node& n );
    Node getNode( sparksee::gdb::oid_t id );
    NodeVec getNode( const ObjectsPtr& objs );

    // Forward to LinkDao
    HLink getHLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    HLink getHLink( sparksee::gdb::oid_t hid );
    bool updateHLink( HLink& link );
    std::vector<HLink> getHLink( const ObjectsPtr& objs );

    VLink getVLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    VLink getVLink( sparksee::gdb::oid_t vid );
    bool updateVLink( VLink& link );

    OLink getOLink( sparksee::gdb::oid_t layerId, sparksee::gdb::oid_t nodeId );
    OLink getOLink( sparksee::gdb::oid_t eid );
    bool updateOLink( OLink& link );
    void removeOLink( sparksee::gdb::oid_t layerId, sparksee::gdb::oid_t nodeId );

    // Forward to LayerDAO
    /**
     * @brief Add base layer, initial layer, mandatory
     * @return base layer
     */
    Layer addBaseLayer();
    /**
     * @brief Switch an existing layer to base layer
     * The previous base layer is set a normal set.
     * There can only be one Base layer in the MLG
     * @param layer
     */
    void setAsBaseLayer( Layer& layer );

    /**
     * @brief Add a Layer on top of the (MLG)
     * Needs a base Layer
     * @return Top layer
     */
    Layer addLayerOnTop();
    /**
     * @brief Add a Layer on bottom of the (MLG)
     * Needs a base Layer
     * @return bottom layer
     */
    Layer addLayerOnBottom();

    /**
     * @brief Remove MLG Top Layer
     * Needs a base layer. All SuperNodes (and associated HLINK, VLINK)
     * owned by the layer are deleted.
     * @return success
     */
    bool removeTopLayer();
    /**
     * @brief Remove MLG bottom Layer
     * Needs a base layer. All SuperNodes (and associated HLINK, VLINK)
     * owned by the layer are deleted.
     * @return success
     */
    bool removeBottomLayer();
    /**
     * @brief Remove base layer
     * Remove the base layer iff there are no other
     * layer in the MLG. All SuperNodes (and associated HLINK, VLINK)
     * owned by the layer are deleted.
     * @return success
     */
    bool removeBaseLayer();
    /**
     * @brief Remove all the layers except the base one.
     * Needs a Base layer. All SuperNodes (and associated HLINK, VLINK)
     * owned by the layer are deleted.
     * @return success
     */
    bool removeAllButBaseLayer();

    /**
     * @brief Get MLG's bottom Layer
     * Needs a base layer
     * @return bottom layer
     */
    Layer bottomLayer();
    /**
     * @brief Get MLG's top Layer
     * Needs a base layer
     * @return top layer
     */
    Layer topLayer();
    /**
     * @brief Get MLG's base Layer
     * @return base layer
     */
    Layer baseLayer();

    /**
     * @brief Get parent layer.
     * Follow the CHILD_OF edge relationship.
     * @param layer Input
     * @return parent Layer
     */
    Layer parent( const Layer& layer );
    sparksee::gdb::oid_t parent( sparksee::gdb::oid_t lid );

    /**
     * @brief Get child layer.
     * Follow the CHILD_OF edge relationship.
     * @param layer Input
     * @return child Layer
     */
    Layer child( const Layer& layer );
    sparksee::gdb::oid_t child( sparksee::gdb::oid_t lid );

    /**
     * @brief Get numbers of layer in the MLG`
     * @return number of layers
     */
    int64_t getLayerCount();
    /**
     * @brief Update Layer attributes
     * @param layer Input layer
     */
    void updateLayer( Layer& layer );

    /**
     * @brief Get layer by id
     * @param id Layer id
     * @return Layer
     */
    Layer getLayer( sparksee::gdb::oid_t id );
    /**
     * @brief Check if layer exists
     * @param layer
     * @return exists
     */
    bool exists( const Layer& layer );

    std::vector<Layer> getAllLayers();

    CLink topCLink( sparksee::gdb::oid_t lid );
    CLink bottomCLink( sparksee::gdb::oid_t lid );
    CLink getCLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    CLink getCLink( sparksee::gdb::oid_t clid );

    bool updateCLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, double weight );
    bool updateCLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, AttrMap& data );
    bool updateCLink( sparksee::gdb::oid_t eid, AttrMap& data );
    bool updateCLink( sparksee::gdb::oid_t eid, double weight );

    sparksee::gdb::type_t hlinkType() const;
    sparksee::gdb::type_t vlinkType() const;
    sparksee::gdb::type_t nodeType() const;
    sparksee::gdb::type_t olinkType() const;

private:
    sparksee::gdb::oid_t getLayerIdForNode( sparksee::gdb::oid_t nid );
    Layer mirrorLayerImpl( Direction dir );
    HLink mirrorEdge( const HLink& current, Direction dir, const Layer& newLayer, NodeMap& nodeMap );
    Node mirrorNode( sparksee::gdb::oid_t current, Direction dir, const Layer& newLayer );
    ObjectsPtr getVLinkEndpoints( sparksee::gdb::oid_t current, Direction dir );
    bool horizontalCopyLinks( sparksee::gdb::type_t linkType,
                              const Node& source,
                              const Node& target,
                              bool safe,
                              const ObjectsPtr& subset,
                              const WeightMergerFunc& f );

private:
    std::unique_ptr<NodeDao> m_node;
    std::unique_ptr<LayerDao> m_layer;
    std::unique_ptr<LinkDao> m_link;
};

} // end namespace mld

#endif // MLD_MLGDAO_H
