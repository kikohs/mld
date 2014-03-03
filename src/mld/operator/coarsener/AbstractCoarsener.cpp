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

#include "mld/operator/coarsener/AbstractCoarsener.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;


AbstractCoarsener::AbstractCoarsener( sparksee::gdb::Graph* g )
    : AbstractOperator()
    , m_dao( new MLGDao(g) )
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

uint64_t AbstractCoarsener::computeMergeCount( int64_t numVertices, bool willUseMirroring )
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
