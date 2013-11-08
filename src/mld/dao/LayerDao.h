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

namespace dex {
namespace gdb {
    class Graph;
    class Value;
}}

namespace mld {
    class SNodeDao;
}

namespace mld {

/**
 * @brief The LayerDao class
 *  Manage access to database Layer objects
 */
class MLD_API LayerDao : public AbstractDao
{
public:
    LayerDao( dex::gdb::Graph* g );
    virtual ~LayerDao() override;

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

    int64_t countLayers();
    bool updateLayer( const Layer& layer );
    Layer getLayer( dex::gdb::oid_t id );
    bool exists( const Layer& layer );

    /**
     * @brief Check filiation
     * @param layer1
     * @param layer2
     * @return TRue is one of the layer is child of the other
     */
    bool affiliated( dex::gdb::oid_t src, dex::gdb::oid_t tgt );

private:
    dex::gdb::oid_t addLayer();
    bool removeLayer( dex::gdb::oid_t lid );

    bool attachOnTop( dex::gdb::oid_t newId );
    bool attachOnBottom( dex::gdb::oid_t newId );

    dex::gdb::oid_t baseLayerImpl();
    void setAsBaseLayerImpl( dex::gdb::oid_t newId );

    dex::gdb::oid_t topLayerImpl();
    dex::gdb::oid_t bottomLayerImpl();

    dex::gdb::oid_t parentImpl( dex::gdb::oid_t lid );
    dex::gdb::oid_t childImpl( dex::gdb::oid_t lid );

private:
    dex::gdb::type_t m_lType;
};

} // end namespace mld

#endif // MLD_LAYERDAO_H
