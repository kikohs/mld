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

void VirtualGraph::addVNode( const VNode& vn )
{
    VNodeId vnid = boost::add_vertex(m_g);
    m_g[vnid] = vn;
    // update index map
    m_index[vn.first][vn.second.id()] = vnid;
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
