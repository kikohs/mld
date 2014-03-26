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

#include "mld/operator/filter/AbstractTimeVertexFilter.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

AbstractTimeVertexFilter::AbstractTimeVertexFilter( Graph* g )
    : m_dao( new MLGDao(g) )
    , m_timeWindowSize(0)
    , m_override(false)
    , m_lambda(0.0)
    , m_timeOnly(false)
    , m_excludedNodes(m_dao->newObjectsPtr())
{
}

AbstractTimeVertexFilter::~AbstractTimeVertexFilter()
{
}

void AbstractTimeVertexFilter::setExcludedNodes( const ObjectsPtr& nodeSet )
{
    m_excludedNodes = nodeSet;
}

void AbstractTimeVertexFilter::overrideInterLayerWeight( double w )
{
    m_override = true;
    m_lambda = w;
}

void AbstractTimeVertexFilter::setTimeWindowSize( uint32_t nbHops )
{
    m_timeWindowSize = nbHops;
}

void AbstractTimeVertexFilter::computeTWCoeffs( oid_t layerId )
{
    m_coeffs.clear();

    // Current layerId coeff
    oid_t curLayerId = layerId;
    double lastLambda = 0.0;
    m_coeffs.push_back(TWCoeff(layerId, lastLambda));

    // No need to compute any coeffs
    if( m_timeWindowSize == 0 )
        return;

    // Bottom layers
    for( uint32_t i = 0; i < m_timeWindowSize; ++i ) {
        CLink link(m_dao->bottomCLink(curLayerId));
        if( link.id() == Objects::InvalidOID )
            break;
        if( link.weight() == 0.0 ) {
            LOG(logERROR) << "AbstractTimeVertexFilter::computeTWCoeffs CLink weight = 0";
            link.setWeight(1.0);
        }
        lastLambda += 1 / link.weight();
        m_coeffs.push_back(TWCoeff(curLayerId, lastLambda));

        // Go down 1 layer
        curLayerId = link.source();
    }

    // Reverse vector to have [bot2, bot1, 1] and pushback upper layers
    std::reverse(m_coeffs.begin(), m_coeffs.end());

    // Reset variable for upper layers
    curLayerId = layerId;
    lastLambda = 0.0;

    // Top layers
    for( uint32_t i = 0; i < m_timeWindowSize; ++i ) {
        CLink link(m_dao->topCLink(curLayerId));
        if( link.id() == Objects::InvalidOID )
            break;
        if( link.weight() == 0.0 ) {
            LOG(logERROR) << "AbstractTimeVertexFilter::computeTWCoeffs CLink weight = 0";
            link.setWeight(1.0);
        }
        lastLambda += 1 / link.weight();
        m_coeffs.push_back(TWCoeff(curLayerId, lastLambda));

        // Go up 1 layer
        curLayerId = link.target();
    }
}

std::ostream& operator <<( std::ostream& out, const mld::AbstractTimeVertexFilter& filter )
{
    out << filter.name();
    return out;
}
