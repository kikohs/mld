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

#ifndef MLD_ABSTRACTCOARSENER_H
#define MLD_ABSTRACTCOARSENER_H

#include "mld/common.h"
#include "mld/operator/AbstractOperator.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
}}

namespace mld {

class NeighborMerger;
class NeighborSelector;
class MLGDao;

class MLD_API AbstractCoarsener : public AbstractOperator
{
public:
    AbstractCoarsener( sparksee::gdb::Graph* g );
    virtual ~AbstractCoarsener() = 0;

    /**
     * @brief Scale Factor, for coarsener
     * The factor is always calculated from the Base layer
     * E.g 0.1 means that the goal is to keep 90% of base layer nodes
     * @param fac
     */
    void setReductionFactor( float fac );
    inline float reductionFactor() const { return m_reductionFac; }
    /**
     * @brief Return the number of nodes to merge
     * @param numVertices base layer num of vertices returned by sparksee as an int64_t
     * by default, all coarseners mirrors the last layer.
     * @return node count
     */
    int64_t computeMergeCount( int64_t numVertices );
    virtual std::string name() const;

protected:
    std::unique_ptr<MLGDao> m_dao;
    float m_reductionFac;
};

} // end namespace mld

std::ostream& operator<<( std::ostream& out, const mld::AbstractCoarsener& coar );

#endif // MLD_ABSTRACTCOARSENER_H
