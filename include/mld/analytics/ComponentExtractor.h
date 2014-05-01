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

#ifndef MLD_COMPONENTEXTRACTOR_H
#define MLD_COMPONENTEXTRACTOR_H

#include <sparksee/gdb/common.h>
#include "mld/common.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
    class OIDList;
}}

namespace mld {

class MLGDao;
class Layer;
class DynamicGraph;

class MLD_API ComponentExtractor
{
    using OIDVec = std::vector<sparksee::gdb::oid_t>;
public:
    explicit ComponentExtractor( sparksee::gdb::Graph* g );
    ComponentExtractor( const ComponentExtractor& ) = delete;
    ComponentExtractor& operator=( ComponentExtractor ) = delete;
    ~ComponentExtractor();

    inline void setOverrideThreshold( bool override, double value ) { m_override = override; m_alphaOverride = value; }
    /**
     * @brief Run the component extractor algorithm:
     * - createDynamicGraph
     * - extractPatterns
     * - layout graph
     * @return success
     */
    bool run();

    inline DynGraphPtr dynGraph() const { return m_dg; }

private:
    /**
     * @brief Create dynamic graph of nodes, each active node on a layer t is connected
     * to its neighbors only if the neighbors are themselves activated on layer t+1.
     * Note: to be active, a node should have its current weight (the associated OLink weight)
     * above a threshold, given by the user or calculated via computeThreshold.
     * The graph minimal degree is 1.
     * @return success
     */
    bool createDynamicGraph();

    void addSelfDyEdges( const Layer& lSrc, const Layer& lTgt, const ObjectsPtr& nodes );

    /**
     * @brief Add edge between 2 node and create Node if it does not exist
     * @param lSrc
     * @param src
     * @param lTgt
     * @param tgt
     */
    void addSafeDyEdge( const Layer& lSrc, sparksee::gdb::oid_t src,
                       const Layer& lTgt, sparksee::gdb::oid_t tgt );


    /**
     * @brief Compute thresholds if not overrided by user
     * By default, it is equal to 0.5 * max(abs(X)). Where is X is the vector of olink weights. for each tsgroup
     * @return threshold
     */
    std::vector<double> computeThresholds( const std::vector<TSGroupId>& groups = std::vector<TSGroupId>() );

    /**
     * @brief Filter nodes of a layer return only those whose TS value for the input layer
     * is above threshold or inferior to - threshold
     * @param layer Current layer
     * @param Timeseries group
     * @param threshold
     * @return nodeset
     */
    ObjectsPtr filterNodes( const Layer& layer, TSGroupId group, double threshold );

    bool extractComponents();
    bool layout();

private:
    std::unique_ptr<MLGDao> m_dao;
    bool m_override;
    double m_alphaOverride;
    std::vector<double> m_alphas;
    DynGraphPtr m_dg;
};

} // end namespace mld

#endif // MLD_COMPONENTEXTRACTOR_H
