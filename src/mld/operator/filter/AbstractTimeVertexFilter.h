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

#ifndef MLD_ABSTRACTTIMEVERTEXFILTER_H
#define MLD_ABSTRACTTIMEVERTEXFILTER_H

#include "mld/common.h"
#include "mld/model/Link.h"

namespace sparksee {
namespace gdb {
    class Graph;
}}

namespace mld {

class MLGDao;

class MLD_API AbstractTimeVertexFilter
{
public:
    AbstractTimeVertexFilter( sparksee::gdb::Graph* g );
    virtual ~AbstractTimeVertexFilter() = 0;

    virtual std::string name() const = 0;

    /**
     * @brief Exclude Nodes, they will not
     * be processed nor retrieved as neighbors
     * @param nodeSet
     */
    void setExcludedNodes( const ObjectsPtr& nodeSet );

    /**
     * @brief Override CLink weight in between layer with given value.
     * Does not touch CLink weight in database
     * @param w new weight
     */
    void overrideInterLayerWeight( double w );

    /**
     * @brief Set the size (or height) for the time window size
     * Default is 1-hop
     * @param nbHops
     */
    void setTimeWindowSize( uint32_t nbHops );

    /**
     * @brief If true, the filter only use signal values on the nodes
     * and does not account for neighbors
     * @param v
     */
    inline void setFilterOnlyInTimeDomain( bool v ) { m_timeOnly = v; }

    /**
     * @brief Get excluded node set, DO NOT DELETE
     * @return excluded nodes
     */
    inline sparksee::gdb::Objects* excludedNodes() { return m_excludedNodes.get(); }

    /**
     * @brief Compute the new value for the current node after the filtering
     * It is stored into the corresponding OLink.weight field
     * @param layerId
     * @param rootId
     * @return new filtered value in weight()
     */
    virtual OLink compute( sparksee::gdb::oid_t layerId, sparksee::gdb::oid_t rootId ) = 0;

    /**
     * @brief Compute TimeWidow coeffs using the resistivity distance
     * @param layerId layer
     */
    virtual void computeTWCoeffs( sparksee::gdb::oid_t layerId );

protected:
    using TWCoeff = std::pair<sparksee::gdb::oid_t, double>;
    using TWCoeffVec = std::vector<TWCoeff>;

    /**
     * @brief Compute weight for a node for a special time scale
     * @param node neighbor node
     * @param hlinkWeight Current edge weight between node and root node
     * @param coeff
     * @return node score
     */
    virtual OLink computeNodeWeight( sparksee::gdb::oid_t node,
                                      double hlinkWeight, const TWCoeff& coeff ) = 0;

protected:
    std::unique_ptr<MLGDao> m_dao;
    uint32_t m_timeWindowSize;
    bool m_override;
    double m_lambda;
    bool m_timeOnly;

    ObjectsPtr m_excludedNodes;
    TWCoeffVec m_coeffs;
};

} // end namespace mld

std::ostream& operator <<( std::ostream& out, const mld::AbstractTimeVertexFilter& filter );

#endif // MLD_ABSTRACTTIMEVERTEXFILTER_H
