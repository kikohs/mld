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
}}

namespace mld {
    class SNodeDao;
    class LayerDao;
}

namespace mld {

/**
 * @brief The MultiLayerGraph dao
 * Main class for accessing the underlying database structure
 */
class MLD_API MLGDao: public AbstractDao
{
public:
    MLGDao( dex::gdb::Graph* g );
    virtual ~MLGDao() override;

    /**
     * @brief Manages Layers via LayerDao
     * DO NOT DELETE
     * @return
     */
    LayerDao* layerHandle() { return m_ldao.get(); }

//    SuperNode addNode( const Layer& layer );
private:
    std::unique_ptr<SNodeDao> m_sndao;
    std::unique_ptr<LayerDao> m_ldao;
};

} // end namespace mld

#endif // MLD_MLGDAO_H
