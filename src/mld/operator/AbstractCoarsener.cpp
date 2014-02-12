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

#include <dex/gdb/Objects.h>

#include "mld/operator/AbstractCoarsener.h"
#include "mld/operator/AbstractSelector.h"
#include "mld/operator/AbstractMerger.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace dex::gdb;


AbstractCoarsener::AbstractCoarsener( dex::gdb::Graph* g )
    : AbstractOperator(g)
    , m_reductionFac(0.0)
{
}

AbstractCoarsener::~AbstractCoarsener()
{
}

void AbstractCoarsener::setReductionFactor( float fac )
{
    if( fac < 0 )
        m_reductionFac = 0;
    else if( fac > 1 )
        m_reductionFac = 1;
    else
        m_reductionFac = fac;
}

uint64_t AbstractCoarsener::computeMergeCount( int64_t numVertices )
{
    if( numVertices < 2 ) {
        LOG(logWARNING) << "AbstractCoarsener::computeMergeCount: need at least 2 nodes";
        return 0;
    }
    // Set merge count
    uint64_t mergeCount = 0;
    // Check scale factor
    if( m_reductionFac == 1.0 ) { // 100% reduction
        LOG(logWARNING) << "AbstractCoarsener::computeMergeCount: reduction factor is 100%, collapsing graph into 1 node";
        mergeCount = numVertices - 1;
    }
    else if( m_reductionFac == 0.0 ) { // No reduction only 1 node
        LOG(logWARNING) << "AbstractCoarsener::computeMergeCount: reduction factor is 0%, coarsening only 1 node";
        mergeCount = 1;
    }
    else {
        mergeCount = m_reductionFac * numVertices + 1;
    }
    return mergeCount;
}


AbstractSingleCoarsener::AbstractSingleCoarsener( dex::gdb::Graph* g )
    : AbstractCoarsener(g)
{
}

AbstractSingleCoarsener::~AbstractSingleCoarsener()
{
}

bool AbstractSingleCoarsener::preExec()
{
    if( !m_sel || !m_merger ) {
        LOG(logERROR) << "AbstractSingleCoarsener::pre_exec: You need to set a \
                         Selector and a Merger in children classes";
        return false;
    }

    // Check base layer node count
    auto numVertices = m_dao->getNodeCount(m_dao->baseLayer());
    if( numVertices < 2 ) {
        LOG(logERROR) << "AbstractSingleCoarsener::pre_exec: invalid base layer contains less than 2 nodes";
        return false;
    }

    // Check current layer node count
    Layer current = m_dao->topLayer();
    numVertices = m_dao->getNodeCount(current);
    if( numVertices < 2 ) {
        LOG(logERROR) << "AbstractSingleCoarsener::pre_exec: current layer contains less than 2 nodes";
        return false;
    }

    Layer top = m_dao->mirrorTopLayer();
    if( top.id() == Objects::InvalidOID )
        return false;

    return true;
}

bool AbstractSingleCoarsener::exec()
{    
    Layer current = m_dao->topLayer();
    auto numVertices = m_dao->getNodeCount(current);
    auto mergeCount = computeMergeCount(numVertices);
    // While there are edge to collapse
    while( mergeCount > 0 ) {
        HLink link = m_sel->selectBestHLink(current);
        if( link.id() != Objects::InvalidOID ) {
            bool success = m_merger->merge(link, *m_sel);
            if( !success ) {
                LOG(logERROR) << "AbstractSingleCoarsener::exec: Merger failed to collapse HLink " << link;
                return false;
            }
            mergeCount--;
        }
        else {
            LOG(logERROR) << "AbstractSingleCoarsener::exec: best HLink is invalid";
            return false;
        }
    }
    return true;
}

bool AbstractSingleCoarsener::postExec()
{
    return true;
}
