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
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

TimeVertexMeanFilter::TimeVertexMeanFilter( Graph* g )
    : AbstractTimeVertexFilter(g)
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

    if( m_timeWindowSize == 0 ) {
        name += " filter on vertex domain only";
        return name;
    }

    name += "timewindow size: " + std::to_string(m_timeWindowSize);
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

//    LOG(logDEBUG) << "TimeVertexMeanFilter::compute: " << rootId;

    double total = 0.0;
    int neighborsCount = 0;
    // Compute weight for root node itself (no hlink, set value to 1)
    for( auto& tw: m_coeffs )
        total += computeNodeWeight(rootId, 1, tw).weight();

    if( !m_timeOnly ) {  // Filter in the vertex domain
        // Get current valid neighbors
        ObjectsPtr neigh(m_dao->graph()->Neighbors(rootId, m_dao->hlinkType(), Outgoing));
        neigh->Difference(m_excludedNodes.get());
        neighborsCount = neigh->Count();

        ObjectsIt it(neigh->Iterator());
        while( it->HasNext() ) {  // Iterate through each neighbors
            oid_t current = it->Next();
            HLink currentHLink(m_dao->getHLink(rootId, current));
            for( auto& tw: m_coeffs ) { // Loop through each layer to get node weight
                total += computeNodeWeight(current, currentHLink.weight(), tw).weight();
            }
        }
    }

    // The final weight is the mean of all weights
    // do not forget to count the root node coeffs
    total /= (m_coeffs.size() * neighborsCount) + m_coeffs.size();
    rootOLink.setWeight(total);

    return rootOLink;
}

OLink TimeVertexMeanFilter::computeNodeWeight( oid_t node, double hlinkWeight, const TWCoeff& coeff )
{
    OLink olink(m_dao->getOLink(coeff.first, node));

#ifdef MLD_SAFE
    if( olink.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "TimeVertexMeanFilter::computeNodeWeight invalid Olink " << coeff.first << " " << node;
        return olink;
    }

    if( hlinkWeight == 0.0 ) {
        LOG(logERROR) << "TimeVertexMeanFilter::computeNodeWeight: HLink weight = 0";
        olink.setWeight(0.0);
        return olink;
    }
#endif
    // Resistivity coeff
    double c = 1 / (1.0 / hlinkWeight + coeff.second);
    olink.setWeight(c * olink.weight());
    return olink;
}
