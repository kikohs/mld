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

#include "mld/common.h"
#include "mld/dao/AbstractDao.h"
#include "mld/model/SuperNode.h"
#include "mld/model/Layer.h"
#include "mld/model/Link.h"

namespace dex {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
}}

namespace mld {
    class SNodeDao;
    class LayerDao;
    class LinkDao;
}

namespace mld {

/**
 * @brief The MultiLayerGraph (MLG) dao
 * Main class for accessing the underlying database structure
 */
class MLD_API MLGDao: public AbstractDao
{
public:
    MLGDao( dex::gdb::Graph* g );
    virtual ~MLGDao() override;

    /**
     * @brief Add a node to a specified Layer
     * @param if MLD_SAFE flag is set, it checks for the existence of the layer
     * @return
     */
    SuperNode addNodeToLayer( const Layer& l );

    /**
     * @brief Add HLink
     * If MLG_SAFE flag is on, checks that the 2 nodes belong to the same layer
     * @param src Supernode source
     * @param tgt Supernode tgt
     * @param weight
     * @return New created HLink
     */
    HLink addHLink( const SuperNode& src, const SuperNode& tgt, double weight = kHLINK_DEF_VALUE );

    /**
     * @brief Add VLink
     * If MLG_SAFE flag is on, checks that the 2 nodes are in adajacent layers
     * @param src Supernode source
     * @param tgt Supernode tgt
     * @param weight
     * @return New created VLink
     */
    VLink addVLink( const SuperNode& src, const SuperNode& tgt, double weight = kVLINK_DEF_VALUE );


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
    std::vector<SuperNode> getAllSuperNode( const Layer& l );

    /**
     * @brief Get all Hlink for an input layer
     * @param l Input layer
     * @return HLinks
     */
    std::vector<HLink> getAllHLinks( const Layer& l );

    // Forward to LinkDao
    HLink getHLink( const SuperNode& src, const SuperNode& tgt );
    HLink getHLink( dex::gdb::oid_t hid );

    VLink getVLink( const SuperNode& src, const SuperNode& tgt );
    VLink getVLink( dex::gdb::oid_t vid );

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
    /**
     * @brief Get child layer.
     * Follow the CHILD_OF edge relationship.
     * @param layer Input
     * @return child Layer
     */
    Layer child( const Layer& layer );

    /**
     * @brief Get numbers of layer in the MLG`
     * @return number of layers
     */
    int64_t countLayers();
    /**
     * @brief Update Layer attributes
     * @param layer Input layer
     * @return success
     */
    bool updateLayer( const Layer& layer );

    /**
     * @brief Get layer by id
     * @param id Layer id
     * @return Layer
     */
    Layer getLayer( dex::gdb::oid_t id );
    /**
     * @brief Check if layer exists
     * @param layer
     * @return exists
     */
    bool exists( const Layer& layer );

private:
    dex::gdb::oid_t getLayerIdForSuperNode( dex::gdb::oid_t nid );

private:
    std::unique_ptr<SNodeDao> m_sn;
    std::unique_ptr<LayerDao> m_layer;
    std::unique_ptr<LinkDao> m_link;
    dex::gdb::type_t m_ownsType;
};

} // end namespace mld

#endif // MLD_MLGDAO_H
