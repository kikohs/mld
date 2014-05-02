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

#include "mld/model/DynamicGraph.h"

using namespace mld;
using namespace sparksee::gdb;

DynamicGraph::DynamicGraph()
    : m_layerLastPos(0)
    , m_componentCount(0)
    , m_groupCount(0)
{
}

void DynamicGraph::addDyNode( oid_t layer, const Node& vn )
{
    DyNodeId vnid = boost::add_vertex(m_g);
    m_g[vnid] = vn;
    AttrMap& d(m_g[vnid].data());
    // Add layer id in vnode data
    d[Attrs::V[DyNodeAttr::LAYERID]].SetLongVoid(layer);
    d[Attrs::V[DyNodeAttr::BASEID]].SetLongVoid(vn.id());

    // If an id exist in the map, modify it to differentiate it from the sparksee id
    auto it = d.find(L"id");
    if( it != d.end() ) {
        std::wstring newId = it->second.GetString();
        d.erase(it);
        // assumes id are numbers
        int nid = std::stoi(newId);
        d[Attrs::V[DyNodeAttr::INPUTID]].SetIntegerVoid(nid);
    }

    // update index map
    m_index[layer][vn.id()] = vnid;

    // Check if the layer is already in the position map
    auto lPosIt = m_layerMap.find(layer);
    if( lPosIt == m_layerMap.end() ) {
        m_layerMap[layer] = m_layerLastPos++;
    }
}

void DynamicGraph::addDyEdge( oid_t lSrc, oid_t src, oid_t lTgt, oid_t tgt )
{
    DyEdgeId edgeId;
    bool success = false;

    // Find VNodeId
    DyNodeId realSrc = m_index.at(lSrc).at(src);
    DyNodeId realTgt = m_index.at(lTgt).at(tgt);

    // Add edge
    boost::tie(edgeId, success) = boost::add_edge(realSrc, realTgt, m_g);

    if( !success ) {
        LOG(logERROR) << "VirtualGraph::addVEdge failed to add edge";
    }
}

std::pair<DyNode, bool> DynamicGraph::getNode( oid_t layer, oid_t src )
{
    auto lIt = m_index.find(layer);
    if( lIt == m_index.end() )  // layer does not exis
        return std::make_pair(DyNode(), false);

     auto nIt = m_index.at(layer).find(src);
     if( nIt == m_index.at(layer).end() ) { // node not found
          return std::make_pair(DyNode(), false);
     }

     return std::make_pair(m_g[nIt->second], true);
}

bool DynamicGraph::exist( oid_t layer, oid_t src )
{
    auto lIt = m_index.find(layer);
    if( lIt == m_index.end() )  // layer does not exis
        return false;

     auto nIt = m_index.at(layer).find(src);
     if( nIt == m_index.at(layer).end() ) { // node not found
          return false;
     }
     return true;
}

std::ostream& operator<<( std::ostream& out, const DynamicGraph& dg )
{
    const DyGraph& g = dg.data();
    out << "Dynamic graph: "
        << "#layers: " << dg.layerMap().size() << " "
        << "#nodes: " << boost::num_vertices(g) << " "
        << "#edges: " << boost::num_edges(g) << " "
        << "#components: " << dg.componentCount() << " "
        ;
    return out;
}
