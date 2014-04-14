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

#include <cmath>
#include <sparksee/gdb/Graph.h>

#include "mld/analytics/ComponentExtractor.h"
#include "mld/dao/MLGDao.h"
#include "mld/model/VirtualGraph.h"

using namespace mld;
using namespace sparksee::gdb;

namespace {
} // end namespace anonymous

ComponentExtractor::ComponentExtractor( Graph* g )
    : m_dao( new MLGDao(g) )
    , m_override(false)
    , m_alpha(0.0)
    , m_vgraph(nullptr)
{
}

ComponentExtractor::~ComponentExtractor()
{
}

VirtualGraphPtr ComponentExtractor::run()
{
    m_vgraph.reset( new VirtualGraph );
    if( !m_override )
        m_alpha = computeThreshold();

    auto layers = m_dao->getAllLayers();

    // Get base layer thresholded nodes and hlinks
    ObjectsPtr currentNodes(filterNodes(layers.at(0), m_alpha));
    ObjectsPtr currentHlinks(m_dao->graph()->Neighbors(currentNodes.get(), m_dao->hlinkType(), Any));

    addVirtualNodes(layers.at(0), currentNodes);

    for( size_t i = 1; i < layers.size(); ++i ) {
        ObjectsPtr nextNodes(filterNodes(layers.at(i), m_alpha));

        // Add next layer nodes in virtual graph
        addVirtualNodes(layers.at(i), nextNodes);

        // Get HLinks for next layer on thresholded nodes
        ObjectsPtr nextHlinks(m_dao->graph()->Neighbors(nextNodes.get(), m_dao->hlinkType(), Any));

        // Get common hlinks
        ObjectsPtr commonHl(Objects::CombineIntersection(currentHlinks.get(), nextHlinks.get()));
        ObjectsIt it(commonHl->Iterator());
        while( it->HasNext() ) {
            oid_t eid = it->Next();
            std::unique_ptr<EdgeData> eData(m_dao->graph()->GetEdgeData(eid));
            oid_t src = eData->GetHead();
            oid_t tgt = eData->GetTail();

            OIDVec srcs;
            OIDVec tgts;
            // Check if any of the hlink ends is in the current layer
            if( currentNodes->Exists(src) )
                srcs.push_back(src);

            if( currentNodes->Exists(tgt) )
                srcs.push_back(tgt);

            // Check if any of the hlink ends is in the next layer
            if( nextNodes->Exists(src) )
                tgts.push_back(src);

            if( nextNodes->Exists(tgt) )
                tgts.push_back(tgt);

            // Add vlinks in the virtual graph
            addVirtualVLinks(layers.at(i-1), srcs, layers.at(i), tgts);

            // Switch to next layer
            currentNodes = nextNodes;
            currentHlinks = nextHlinks;
        }
    }

    return m_vgraph;
}

double ComponentExtractor::computeThreshold()
{
    auto attr = m_dao->graph()->FindAttribute(m_dao->olinkType(), Attrs::V[OLinkAttr::WEIGHT]);
    std::unique_ptr<AttributeStatistics> stats(m_dao->graph()->GetAttributeStatistics(attr, true));

    double maxValue = stats->GetMax().GetDouble();
    double minValue = stats->GetMin().GetDouble();

    // 0.5 * max(abs(X))
    return std::max(std::abs(maxValue), std::abs(minValue)) / 2.0;
}

ObjectsPtr ComponentExtractor::filterNodes( const Layer& layer, double threshold )
{
    auto attr = m_dao->graph()->FindAttribute(m_dao->olinkType(), Attrs::V[OLinkAttr::WEIGHT]);
    ObjectsPtr olinks(m_dao->graph()->Explode(layer.id(), m_dao->olinkType(), Outgoing));
    Value v;
    v.SetDoubleVoid(threshold);
    // Select on restricted edge set
    ObjectsPtr filterOl(m_dao->graph()->Select(attr, GreaterEqual, v, olinks.get()));
    // Get olinks with value x <= -threshold
    v.SetDoubleVoid(-threshold);
    filterOl->Union(m_dao->graph()->Select(attr, LessEqual, v, olinks.get()));

    // Return targets of olinks (Nodes)
    return ObjectsPtr(m_dao->graph()->Tails(filterOl.get()));
}

void ComponentExtractor::addVirtualNodes( const Layer& layer, const ObjectsPtr& nodes )
{
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() )
        m_vgraph->addVNode(VNode(layer.id(), m_dao->getNode(it->Next())));
}

void ComponentExtractor::addVirtualVLinks( const Layer& lSrc, const OIDVec& srcs, const Layer& lTgt, const OIDVec& tgts )
{
    for( auto src: srcs ) {
        for( auto tgt: tgts ) {
            m_vgraph->addVEdge(lSrc.id(), src, lTgt.id(), tgt);
        }
    }
}
