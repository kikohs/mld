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
#include "mld/model/Layer.h"

namespace dex {
namespace gdb {
    class Graph;
    class Value;
}}

namespace mld {

/**
 * @brief The LayerDao class
 *  Manage access to database Layer objects
 */
class MLD_API LayerDao
{
public:
    LayerDao( dex::gdb::Graph* g );
    ~LayerDao();

    int countLayers();

    Layer addLayerOnTop();
    Layer addLayerOnBottom();

    void updateLayer( const Layer& layer );
    Layer getLayer( dex::gdb::oid_t id );

    void removeTopLayer();
    void removeBottomLayer();
    Layer setAsBaseLayer( const Layer& layer );

    Layer bottomLayer();
    Layer topLayer();
    Layer baseLayer();

    std::vector<Layer> traverseBottomUp();
    std::vector<Layer> traverseTopDown();

private:
    Layer addLayer();
    void removeLayer( const Layer& layer );

    dex::gdb::oid_t getBaseLayerImpl();
    void setAsBaseLayerImpl( dex::gdb::oid_t newId );

private:
    dex::gdb::Graph* m_g;
    std::unique_ptr<dex::gdb::Value> m_v;
    dex::gdb::type_t m_lType;
};

} // end namespace mld
#endif // MLD_LAYERDAO_H
