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

#include "mld/model/VirtualGraph.h"

using namespace mld;
using namespace sparksee::gdb;

VirtualGraph::VirtualGraph()
{
}

void VirtualGraph::addVNode( oid_t layer, const Node& vn )
{
    VNodeId vnid = boost::add_vertex(m_g);
    m_g[vnid] = vn;
    // Add layer id in vnode data
    m_g[vnid].data()[Attrs::V[VNodeAttr::LAYERID]].SetLongVoid(layer);
    m_g[vnid].data()[Attrs::V[VNodeAttr::BASEID]].SetLongVoid(vn.id());
    // update index map
    m_index[layer][vn.id()] = vnid;
}

void VirtualGraph::addVEdge( oid_t lSrc, oid_t src, oid_t lTgt, oid_t tgt )
{
    VEdgeId edgeId;
    bool success = false;

    // Find VNodeId
    VNodeId realSrc = m_index.at(lSrc).at(src);
    VNodeId realTgt = m_index.at(lTgt).at(tgt);

    // Add edge
    boost::tie(edgeId, success) = boost::add_edge(realSrc, realTgt, m_g);

    if( !success ) {
        LOG(logERROR) << "VirtualGraph::addVEdge failed to add edge";
    }
}
