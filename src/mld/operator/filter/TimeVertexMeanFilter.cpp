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
#include "mld/operator/filter/TimeVertexMeanFilter.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

using TWCoeff = std::pair<oid_t, double>;
using TWCoeffVec = std::vector<TWCoeff>;

namespace {

TWCoeffVec computeTimeWindowCoeffs( oid_t layerId, uint32_t wSize, MLGDao* dao )
{
    // TODO
    return TWCoeffVec();
}

double computeNodeWeight( oid_t current, double hlinkWeight, const TWCoeff& coeff, MLGDao* dao )
{
    // TODO
    return 0.0;
}

} // end namespace anonymous

TimeVertexMeanFilter::TimeVertexMeanFilter( sparksee::gdb::Graph* g )
    : AbstractVertexFilter(g)
{
}

TimeVertexMeanFilter::~TimeVertexMeanFilter()
{
}

std::string TimeVertexMeanFilter::name() const
{
    std::string name("TimeVertexMeanFilter: ");
    name += "timewindow size: " + std::to_string(m_timeWindowSize);
    if( m_override )
        name += " overriden lambda: " + std::to_string(m_lambda);

    return name;
}

OLink TimeVertexMeanFilter::compute( oid_t layerId, oid_t rootId )
{
    // Get current valid neighbors
    ObjectsPtr neigh(m_dao->graph()->Neighbors(rootId, m_dao->hlinkType(), Outgoing));
    neigh->Difference(m_excludedNodes.get());

    TWCoeffVec coeffs(computeTimeWindowCoeffs(layerId, m_timeWindowSize, m_dao.get()));
    double total = 0.0;

    ObjectsIt it(neigh->Iterator());
    while( it->HasNext() ) {  // Iterate through each neighbors
        oid_t current = it->Next();
        HLink currentHLink(m_dao->getHLink(rootId, current));
        for( auto& tw: coeffs )  // Loop through each layer to get node weight
            total += computeNodeWeight(current, currentHLink.weight(), tw, m_dao.get());
    }

    // Compute weight for root node itself (no hlink, set value to 1)
    for( auto& tw: coeffs )
        total += computeNodeWeight(rootId, 1, tw, m_dao.get());

    // The final weight is the mean of all weights
    // do not forget to count the root node coeffs
    total /= (coeffs.size() * neigh->Count()) + coeffs.size();

    OLink rootOLink(m_dao->getOLink(layerId, rootId));
    rootOLink.setWeight(total);

    return rootOLink;
}
