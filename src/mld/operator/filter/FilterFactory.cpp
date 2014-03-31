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

#include <boost/algorithm/string.hpp>

#include "mld/operator/filter/FilterFactory.h"
#include "mld/operator/filter/TimeVertexMeanFilter.h"

using namespace mld;
namespace ba = boost::algorithm;

AbstractTimeVertexFilter* FilterFactory::create( sparksee::gdb::Graph* g, const std::string& name, double lambda, uint32_t twSize )
{
    AbstractTimeVertexFilter* filter = nullptr;
    auto s = ba::to_lower_copy(name);
    if( s == "tvm" ) {
        filter = new TimeVertexMeanFilter(g);
        filter->setTimeWindowSize(twSize);
        if( lambda > 0.0 )
            filter->setOverrideInterLayerWeight(true, lambda);
    }
    return filter;
}
