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

#ifndef MLD_LAYERDAO_H
#define MLD_LAYERDAO_H

#include "mld/common.h"
#include "mld/dao/AbstractDao.h"
#include "mld/model/Layer.h"
#include "mld/model/Link.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
}}

namespace mld {
    class NodeDao;
}

namespace mld {

/**
 * @brief The LayerDao class
 *  Manage access to database Layer objects
 */
class MLD_API LayerDao : public AbstractDao
{
public:
    LayerDao( sparksee::gdb::Graph* g );
    virtual ~LayerDao() override;
    virtual void setGraph( sparksee::gdb::Graph* g ) override;

    Layer addBaseLayer();
    void setAsBaseLayer( Layer& layer );

    Layer addLayerOnTop();
    Layer addLayerOnBottom();

    bool removeTopLayer();
    bool removeBottomLayer();
    bool removeBaseLayer();
    bool removeAllButBaseLayer();

    Layer bottomLayer();
    Layer topLayer();
    Layer baseLayer();

    Layer parent( const Layer& layer );
    Layer child( const Layer& layer );

    int64_t getLayerCount();
    void updateLayer( Layer& layer );
    Layer getLayer( sparksee::gdb::oid_t id );
    bool exists( const Layer& layer );

    std::vector<Layer> getAllLayers();

    /**
     * @brief Check filiation
     * @param layer1
     * @param layer2
     * @return TRue is one of the layer is child of the other
     */
    bool affiliated( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );

    CLink topCLink( sparksee::gdb::oid_t lid );
    CLink bottomCLink( sparksee::gdb::oid_t lid );
    CLink getCLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    CLink getCLink( sparksee::gdb::oid_t clid );

    bool updateCLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, double weight );
    bool updateCLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, AttrMap& data );
    bool updateCLink( sparksee::gdb::oid_t eid, AttrMap& data );
    bool updateCLink( sparksee::gdb::oid_t eid, double weight );

private:
    sparksee::gdb::oid_t addLayer();
    bool removeLayer( sparksee::gdb::oid_t lid );

    bool attachOnTop( sparksee::gdb::oid_t newId );
    bool attachOnBottom( sparksee::gdb::oid_t newId );

    sparksee::gdb::oid_t baseLayerImpl();
    void setAsBaseLayerImpl( sparksee::gdb::oid_t newId );

    sparksee::gdb::oid_t topLayerImpl();
    sparksee::gdb::oid_t bottomLayerImpl();

    sparksee::gdb::oid_t parentImpl( sparksee::gdb::oid_t lid );
    sparksee::gdb::oid_t childImpl( sparksee::gdb::oid_t lid );

private:
    sparksee::gdb::type_t m_layerType;
    sparksee::gdb::type_t m_clinkType;
    AttrMap m_layerAttr;
    AttrMap m_clinkAttr;
};

} // end namespace mld

#endif // MLD_LAYERDAO_H
