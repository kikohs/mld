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

#ifndef MLD_TIMEVERTEXMEANFILTER_H
#define MLD_TIMEVERTEXMEANFILTER_H

#include "mld/common.h"
#include "mld/operator/filter/AbstractTimeVertexFilter.h"

namespace sparksee {
namespace gdb {
    class Graph;
}}

namespace mld {

class MLD_API TimeVertexMeanFilter : public AbstractTimeVertexFilter
{

public:
    TimeVertexMeanFilter( sparksee::gdb::Graph* g );
    virtual ~TimeVertexMeanFilter();

    virtual std::string name() const override;
    virtual OLink compute( sparksee::gdb::oid_t layerId, sparksee::gdb::oid_t rootId ) override;
    virtual OLink computeNodeWeight( sparksee::gdb::oid_t node, double hlinkWeight, const TWCoeff& coeff ) override;
};

} // end namespace mld

#endif // MLD_TIMEVERTEXMEANFILTER_H
