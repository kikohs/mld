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

#ifndef MLD_DYNAMICGRAPH_H
#define MLD_DYNAMICGRAPH_H

#include <unordered_map>
#include <boost/graph/adjacency_list.hpp>

#include "mld/common.h"
#include "mld/model/Node.h"

namespace mld {

using DyNode = Node;
using DyGraph = boost::adjacency_list<boost::vecS, // edge container
                                     boost::vecS, // node container
                                     boost::bidirectionalS, // type of the graph
                                     DyNode, // VertexType
                                     boost::no_property>;

using DyNodeId = DyGraph::vertex_descriptor;
using DyEdgeId = DyGraph::edge_descriptor;
using DyGraphTraits = boost::graph_traits<DyGraph>;
using DyOutNodeIter = DyGraphTraits::adjacency_iterator;
using DyOutEdgeIter = DyGraphTraits::out_edge_iterator;
using DyInEdgeIter = DyGraphTraits::in_edge_iterator;
using DyEdgeIter = DyGraphTraits::edge_iterator;
using DyNodeIter = DyGraphTraits::vertex_iterator;
using DyIndexMap = boost::property_map<DyGraph, boost::vertex_index_t>::type;

/// Map Node oid to VNodeId
using LayerIndex = std::unordered_map<sparksee::gdb::oid_t, DyNodeId>;
/// Map Layer to LayerIndex in order
using GraphIndex = std::unordered_map<sparksee::gdb::oid_t, LayerIndex>;
/// Map layers id to their respective index to their position (base is 0 for instance)
using LayerMap = std::map<sparksee::gdb::oid_t, int64_t>;

class MLD_API DynamicGraph
{
public:
    DynamicGraph();
    void addDyNode( sparksee::gdb::oid_t layer, const Node& vn );
    void addDyEdge( sparksee::gdb::oid_t lSrc, sparksee::gdb::oid_t src,
                   sparksee::gdb::oid_t lTgt, sparksee::gdb::oid_t tgt );

    std::pair<DyNode, bool> getNode( sparksee::gdb::oid_t layer, sparksee::gdb::oid_t src );
    bool exist( sparksee::gdb::oid_t layer, sparksee::gdb::oid_t src );

    inline DyGraph& data() { return m_g; }
    inline const DyGraph& data() const { return m_g; }

    inline GraphIndex& index() { return m_index; }
    inline const GraphIndex& index() const { return m_index; }

    inline LayerMap& layerMap() { return m_layerMap; }
    inline const LayerMap& layerMap() const { return m_layerMap; }

    inline int32_t patternCount() const { return m_patternCount; }
    void setPatternCount( int32_t count ) { m_patternCount = count; }

private:
    int64_t m_layerLastPos;
    int32_t m_patternCount;
    GraphIndex m_index;
    DyGraph m_g;
    LayerMap m_layerMap;
};

} // end namespace mld

#endif // MLD_DYNAMICGRAPH_H
