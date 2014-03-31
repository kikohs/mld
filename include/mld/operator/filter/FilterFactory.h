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

#ifndef MLD_FILTERFACTORY_H
#define MLD_FILTERFACTORY_H

#include "mld/common.h"

namespace sparksee {
namespace gdb {
    class Graph;
}}

namespace mld {

class AbstractTimeVertexFilter;

class MLD_API FilterFactory
{
public:
    /**
     * @brief Create a filter from name and parameters
     * @param g Graph
     * @param name Name of the filter
     * @param lambda if lambda > 0.0 the default lambda is overriden
     * @param twSize TimeWindow Size
     * @return filter
     */
    static AbstractTimeVertexFilter* create( sparksee::gdb::Graph* g,
                                             const std::string& name,
                                             double lambda, uint32_t twSize );
};

} // end namespace mld

#endif // MLD_FILTERFACTORY_H
