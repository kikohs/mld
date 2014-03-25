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

#ifndef MLD_ABSTRACTVERTEXFILTER_H
#define MLD_ABSTRACTVERTEXFILTER_H

#include "mld/common.h"
#include "mld/model/Link.h"

namespace sparksee {
namespace gdb {
    class Graph;
}}

namespace mld {

class MLGDao;

class MLD_API AbstractVertexFilter
{
public:
    AbstractVertexFilter( sparksee::gdb::Graph* g );
    virtual ~AbstractVertexFilter() = 0;

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
    void setTimeWindowSize( int nbHops );

    /**
     * @brief Get excluded node set, DO NOT DELETE
     * @return excluded nodes
     */
    sparksee::gdb::Objects* excludedNodes() { return m_excludedNodes.get(); }

    virtual OLink compute( sparksee::gdb::oid_t nodeId, sparksee::gdb::oid_t layerId ) = 0;

protected:
    std::unique_ptr<MLGDao> m_dao;
    int m_timeWindowSize;
    bool m_override;
    double m_lambda;

    ObjectsPtr m_excludedNodes;
};

} // end namespace mld

#endif // MLD_ABSTRACTVERTEXFILTER_H
