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

#ifndef MLD_VIRTUALGRAPH_H
#define MLD_VIRTUALGRAPH_H

#include <unordered_map>
#include <boost/graph/adjacency_list.hpp>

#include "mld/common.h"
#include "mld/model/Node.h"

namespace mld {

using VNode = std::pair<sparksee::gdb::oid_t, Node>;
using VGraph = boost::adjacency_list<boost::vecS, // edge container
                                     boost::vecS, // node container
                                     boost::undirectedS, // type of the graph
                                     VNode, // VertexType
                                     boost::no_property>;

using VNodeId = VGraph::vertex_descriptor;
using VEdgeId = VGraph::edge_descriptor;
using VGraphTraits = boost::graph_traits<VGraph>;
using VAdjIter = VGraphTraits::adjacency_iterator;
using VOutEdgeIter = VGraphTraits::out_edge_iterator;
using VEdgeIter = VGraphTraits::edge_iterator;
using VNodeIter = VGraphTraits::vertex_iterator;
using VIndexMap = boost::property_map<VGraph, boost::vertex_index_t>::type;

// Map Node oid to boost VGraph
using LayerIndex = std::unordered_map<sparksee::gdb::oid_t, VNodeId>;
// Map Layer to LayerIndex in order
using GraphIndex = std::map<sparksee::gdb::oid_t, LayerIndex>;

class MLD_API VirtualGraph
{
public:
    VirtualGraph();
    void addVNode( const VNode& vn );
    void addVEdge( sparksee::gdb::oid_t lSrc, sparksee::gdb::oid_t src,
                   sparksee::gdb::oid_t lTgt, sparksee::gdb::oid_t tgt );

    inline VGraph& data() { return m_g; }
    inline const VGraph& data() const { return m_g; }

    inline GraphIndex& index() { return m_index; }
    inline const GraphIndex& index() const { return m_index; }

private:
    GraphIndex m_index;
    VGraph m_g;
};

} // end namespace mld

std::ostream& operator <<( std::ostream& out, const mld::VNode& vn );

#endif // MLD_VIRTUALGRAPH_H
