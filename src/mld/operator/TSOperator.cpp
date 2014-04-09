/****************************************************************************
**
** Copyright (C) 2014 EPFL-LTS2
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

#include <sparksee/gdb/Graph.h>
#include <sparksee/gdb/Objects.h>
#include <sparksee/gdb/ObjectsIterator.h>

#include "mld/operator/TSOperator.h"
#include "mld/dao/MLGDao.h"
#include "mld/operator/filter/AbstractTimeVertexFilter.h"
#include "mld/utils/Timer.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;

TSOperator::TSOperator( Graph* g )
    : m_dao(new MLGDao(g))
    , m_cache(new TSCache(m_dao))
    , m_filt(nullptr)
{
}

TSOperator::~TSOperator()
{
}

void TSOperator::setFilter( AbstractTimeVertexFilter* filter )
{
    m_filt.reset(filter);
}

bool TSOperator::preExec()
{
    if( !m_filt ) {
        LOG(logERROR) << "TSOperator::preExec: No filter set. Please set a filter first";
        return false;
    }
    return true;
}

bool TSOperator::exec()
{    
    std::unique_ptr<Timer> t(new Timer("TSOperator::exec"));
    m_buffer.clear();
    // Select all nodes from base layer
    ObjectsPtr nodes(m_dao->getAllNodeIds(m_dao->baseLayer()));
    // Remove excluded nodes
    nodes->Difference(m_filt->excludedNodes());

    // Get all layers
    std::vector<Layer> layers(m_dao->getAllLayers());
    size_t oLinkCount = layers.size() * nodes->Count();
    m_buffer.reserve(oLinkCount);

    // Setup cache
    Layer base = m_dao->baseLayer();
    m_cache->reset(base.id(), m_filt->direction(), m_filt->radius());
    m_filt->setCache(m_cache);

    LOG(logINFO) << "Start filtering, " << oLinkCount << " timeseries values to process";
    LOG(logINFO) << *m_filt;
    ProgressDisplay display(oLinkCount);

    for( auto& layer: layers ) {
        // Generate filter coefficient for this layer
        m_filt->computeTWCoeffs(layer.id());
        ObjectsIt it(nodes->Iterator());
        while( it->HasNext() ) {
            oid_t nid = it->Next();
            OLink olink( m_filt->compute(layer.id(), nid) );
#ifdef MLD_SAFE
            if( olink.id() == Objects::InvalidOID ) {
                LOG(logERROR) << "TSOperator::exec invalid OLink";
                return false;
            }
#endif
            m_buffer.push_back(olink);
            ++display;
        }
        m_cache->scrollUp();
    }
    return true;
}

bool TSOperator::postExec()
{
    std::unique_ptr<Timer> t(new Timer("TSOperator::postExec"));
    LOG(logINFO) << "Commit OLink in-memory stored values in DB";
    ProgressDisplay display(m_buffer.size());

    for( auto& olink: m_buffer ) {
        if( !m_dao->updateOLink(olink) ) {
            LOG(logERROR) << "TSOperator::postExec: updateOLink failed: " << olink;
            return false;
        }
        ++display;
    }
    return true;
}
