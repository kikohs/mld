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

#include <vector>
#include <algorithm>
#include "mld/operator/filter/TimeVertexMeanFilter.h"
#include "mld/operator/TSCache.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

TimeVertexMeanFilter::TimeVertexMeanFilter( Graph* g )
    : AbstractTimeVertexFilter(g)
    , m_weightSum(0.0)
{
}

TimeVertexMeanFilter::~TimeVertexMeanFilter()
{
}

std::string TimeVertexMeanFilter::name() const
{
    std::string name("TimeVertexMeanFilter: ");
    if( m_timeOnly ) {
        name += " filter on time domain only";
        return name;
    }

    if( m_radius == 0 ) {
        name += " filter on vertex domain only";
        return name;
    }

    name += "radius: " + std::to_string(m_radius);

    name += " direction: ";
    switch(m_dir) {
        case TSDirection::PAST:
            name += "past";
            break;
        case TSDirection::FUTURE:
            name += "future";
            break;
        case TSDirection::BOTH:
            name += "both";
            break;
        default:
            LOG(logERROR) << "Unsupported direction";
            break;
    }

    if( m_override )
        name += " overriden lambda: " + std::to_string(m_lambda);

    return name;
}

OLink TimeVertexMeanFilter::compute( oid_t layerId, oid_t rootId )
{
    OLink rootOLink(m_dao->getOLink(layerId, rootId));
    if( m_coeffs.empty() ) {
        LOG(logERROR) << "TimeVertexMeanFilter::compute empty coeff, call computeTWCoeffs prior to compute";
        return rootOLink;
    }

    if( rootOLink.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "TimeVertexMeanFilter::compute invalid Olink " << layerId << " " << rootId;
        return rootOLink;
    }

    m_weightSum = 0.0; // weighted sum of all coeffs
    double total = 0.0;
    int neighborsCount = 0;
    // Compute weight for root node itself (no hlink, set value to 1)
    total += computeNodeSelfWeight(rootId);

    if( !m_timeOnly ) {  // Filter in the vertex domain
        // Get current valid neighbors
        ObjectsPtr neigh(m_dao->graph()->Neighbors(rootId, m_dao->hlinkType(), Outgoing));
        neigh->Difference(m_excludedNodes.get());
        neighborsCount = neigh->Count();

        ObjectsIt it(neigh->Iterator());
        while( it->HasNext() ) {  // Iterate through each neighbors
            oid_t current = it->Next();
            HLink currentHLink(m_dao->getHLink(rootId, current));
            total += computeNodeWeight(current, currentHLink.weight());
        }
    }

#ifdef MLD_SAFE
    if( m_weightSum == 0.0 ) {
        LOG(logERROR) << "TimeVertexMeanFilter::compute invalid weighted sum";
        m_weightSum = 1.0;
        return rootOLink;
    }
#endif

    total /= m_weightSum;
    rootOLink.setWeight(total);

    return rootOLink;
}

double TimeVertexMeanFilter::computeNodeWeight( oid_t node, double hlinkWeight )
{
    double total = 0.0;

    if( m_cache ) { // use cache and TimeSeries
        // Get TimeSeries
        auto entry = m_cache->get(node);
        size_t i = 0;
        for( auto it = entry.second.sliceBegin(); it != entry.second.sliceEnd(); ++it ) {
            // Resistivity coeff
            double c = 1.0 / (1.0 / hlinkWeight + m_coeffs.at(i).second);
            m_weightSum += c;
            total += c * (*it);
            ++i;
        }
    }
    else {  // No cache
        for( auto& coeff: m_coeffs ) {
            OLink olink(m_dao->getOLink(coeff.first, node));

#ifdef MLD_SAFE
            if( olink.id() == Objects::InvalidOID ) {
                LOG(logERROR) << "TimeVertexMeanFilter::computeNodeWeight invalid Olink " << coeff.first << " " << node;
                return 0.0;
            }

            if( hlinkWeight == 0.0 ) {
                LOG(logERROR) << "TimeVertexMeanFilter::computeNodeWeight: HLink weight = 0";
                return 0.0;
            }
#endif
            // Resistivity coeff
            double c = 1.0 / (1.0 / hlinkWeight + coeff.second);
            m_weightSum += c;
            total += c * olink.weight();
        }
    }

    return total;
}

double TimeVertexMeanFilter::computeNodeSelfWeight( oid_t node )
{
    double total = 0.0;
    if( m_cache ) {
        // Get TimeSeries
        auto entry = m_cache->get(node);
        size_t i = 0;
        for( auto it = entry.second.sliceBegin(); it != entry.second.sliceEnd(); ++it ) {
            double c = 1.0;
            if( m_coeffs.at(i).second != 0.0 ) {
                c = 1.0 / m_coeffs.at(i).second;
            }
            m_weightSum += c;
            total += c * (*it);
            ++i;
        }
    }
    else {  // No cache
        for( auto& coeff: m_coeffs ) {
            OLink olink(m_dao->getOLink(coeff.first, node));

        #ifdef MLD_SAFE
            if( olink.id() == Objects::InvalidOID ) {
                LOG(logERROR) << "TimeVertexMeanFilter::computeNodeSelfWeight invalid Olink " << coeff.first << " " << node;
                return 0.0;
            }
        #endif
            // Resistivity coeff

            // Special value for self value at current time
            double c = 1.0;
            if( coeff.second != 0.0 ) {
                c = 1.0 / coeff.second;
            }
            m_weightSum += c;
            total += c * olink.weight();
        }
    }

    return total;
}
