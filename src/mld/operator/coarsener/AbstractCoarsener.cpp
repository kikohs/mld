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

#include <sparksee/gdb/Objects.h>

#include "mld/operator/coarsener/AbstractCoarsener.h"
#include "mld/operator/selector/AbstractSelector.h"
#include "mld/operator/merger/AbstractMerger.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;


AbstractCoarsener::AbstractCoarsener( sparksee::gdb::Graph* g )
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

uint64_t AbstractCoarsener::computeMergeCount(int64_t numVertices, bool willUseMirroring )
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
        mergeCount = numVertices;
    }
    else if( m_reductionFac == 0.0 ) { // No reduction only 1 node
        LOG(logWARNING) << "AbstractCoarsener::computeMergeCount: reduction factor is 0%, coarsening only 1 node";
        mergeCount = 1;
    }
    else {
        mergeCount = m_reductionFac * numVertices + 1;
    }

    if( willUseMirroring ) // multi pass has 1 mergeCount iteration for currentLayer pass
        return std::min(mergeCount, uint64_t(numVertices - 1));

    return mergeCount;
}

std::string AbstractCoarsener::name() const
{
    return std::string("AbstractCoarserner");
}

std::ostream& operator<<( std::ostream& out, const mld::AbstractCoarsener& coar )
{
    out << coar.name() << " fac:"
        << coar.reductionFactor() << " "
        ;
    return out;
}

//
// AbstractSingleCoarsener implementation
//

AbstractSingleCoarsener::AbstractSingleCoarsener( sparksee::gdb::Graph* g )
    : AbstractCoarsener(g)
{
}

AbstractSingleCoarsener::~AbstractSingleCoarsener()
{
}

bool AbstractSingleCoarsener::preExec()
{
    if( !m_sel || !m_merger ) {
        LOG(logERROR) << "AbstractSingleCoarsener::preExec: You need to set a \
                         Selector and a Merger in children classes";
        return false;
    }

    // Check base layer node count
    auto numVertices = m_dao->getNodeCount(m_dao->baseLayer());
    if( numVertices < 2 ) {
        LOG(logERROR) << "AbstractSingleCoarsener::preExec: invalid base layer contains less than 2 nodes";
        return false;
    }

    // Check current layer node count
    Layer current = m_dao->topLayer();
    numVertices = m_dao->getNodeCount(current);
    if( numVertices < 2 ) {
        LOG(logERROR) << "AbstractSingleCoarsener::preExec: current layer contains less than 2 nodes";
        return false;
    }

    Layer top = m_dao->mirrorTopLayer();
    if( top.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "AbstractSingleCoarsener::exec: mirroiring failed";
        return false;
    }

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
            if( !m_merger->merge(link, *m_sel) ) {
                LOG(logERROR) << "AbstractSingleCoarsener::exec: Merger failed to collapse HLink " << link;
                return false;
            }
            LOG(logDEBUG) << "AbstractSingleCoarsener::exec: Collapsed HLink: " << link;
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
