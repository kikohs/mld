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
#include "mld/utils/Timer.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;

const int NODE_X_SPACING = 50;
const int NODE_Y_SPACING = 20;

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

bool ComponentExtractor::run()
{
    if( createVGraph() )
        return layout();

    return false;
}

bool ComponentExtractor::createVGraph()
{
    std::unique_ptr<Timer> t(new Timer("Extracting virtual graph"));
    LOG(logINFO) << "Extracting virtual graph";
    m_vgraph.reset( new VirtualGraph );
    if( !m_override )
        m_alpha = computeThreshold();

    auto layers = m_dao->getAllLayers();

    // Get base layer thresholded nodes and hlinks
    ObjectsPtr currentNodes(filterNodes(layers.at(0), m_alpha));
    ObjectsPtr currentHlinks(m_dao->graph()->Explode(currentNodes.get(), m_dao->hlinkType(), Any));

    ProgressDisplay display(layers.size());

    if( currentNodes->Count() != 0 )
        m_vgraph->layerMap()[layers.at(0).id()] = 0;
    addVirtualNodes(layers.at(0), currentNodes);
    ++display;

    for( size_t i = 1; i < layers.size(); ++i ) {
        ObjectsPtr nextNodes(filterNodes(layers.at(i), m_alpha));

        // Add next layer nodes in virtual graph
        if( nextNodes->Count() != 0 )
            m_vgraph->layerMap()[layers.at(i).id()] = i;
        addVirtualNodes(layers.at(i), nextNodes);

        // Add self vlinks between nodes activated in the two layers
        ObjectsPtr commonNodes(Objects::CombineIntersection(currentNodes.get(), nextNodes.get()));
        addVirtualSelfVLinks(layers.at(i-1), layers.at(i), commonNodes);

        // Get HLinks for next layer on thresholded nodes
        ObjectsPtr nextHlinks(m_dao->graph()->Explode(nextNodes.get(), m_dao->hlinkType(), Any));
        // Get common hlinks
        ObjectsPtr commonHl(Objects::CombineIntersection(currentHlinks.get(), nextHlinks.get()));
        ObjectsIt it(commonHl->Iterator());
        while( it->HasNext() ) {
            oid_t eid = it->Next();
            std::unique_ptr<EdgeData> eData(m_dao->graph()->GetEdgeData(eid));
            oid_t src = eData->GetTail();
            oid_t tgt = eData->GetHead();

            // Create VLinks
            if( currentNodes->Exists(src) ) {
                if( nextNodes->Exists(tgt) )
                    m_vgraph->addVEdge(layers.at(i-1).id(), src, layers.at(i).id(), tgt);
            }

            if( currentNodes->Exists(tgt) ) {
                if( nextNodes->Exists(src) )
                    m_vgraph->addVEdge(layers.at(i-1).id(), tgt, layers.at(i).id(), src);
            }
        }
        // Switch to next layer
        currentNodes = nextNodes;
        currentHlinks = nextHlinks;

        ++display;
    }

    return true;
}

void ComponentExtractor::addVirtualNodes( const Layer& layer, const ObjectsPtr& nodes )
{
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() ) {
        m_vgraph->addVNode(layer.id(), m_dao->getNode(it->Next()));
    }
}

void ComponentExtractor::addVirtualSelfVLinks( const Layer& lSrc, const Layer& lTgt, const ObjectsPtr& nodes )
{
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() ) {
        auto nid = it->Next();
        m_vgraph->addVEdge(lSrc.id(), nid, lTgt.id(), nid);
    }
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
    ObjectsPtr filterOl2(m_dao->graph()->Select(attr, LessEqual, v, olinks.get()));
    filterOl->Union(filterOl2.get());

    // Return targets of olinks (Nodes)
    return ObjectsPtr(m_dao->graph()->Heads(filterOl.get()));
}

bool ComponentExtractor::layout()
{
    std::unique_ptr<Timer> t(new Timer("Layouting virtual graph"));
    LOG(logINFO) << "Layouting virtual graph";
    if( !m_vgraph ) {
        LOG(logERROR) << "ComponentExtractor::layout: vgraph is null";
        return false;
    }

    VGraph& g = m_vgraph->data();
    GraphIndex& gIdx = m_vgraph->index();

    ProgressDisplay display(gIdx.size());

    uint32_t lCount = 0;
    for( auto& lp: m_vgraph->layerMap() ) {  // iterate through layer in correct order
        uint32_t nodeLayoutIdx = 0;
        for( auto& vnPair: gIdx.at(lp.first) ) { // for each layer in index
            VNode& vn = g[vnPair.second]; // get bundled property
            // override id with VGraph NodeId
            vn.setId(static_cast<oid_t>(vnPair.second));
            vn.data()[Attrs::V[VNodeAttr::LAYERPOS]].SetLongVoid(lp.second);
            vn.data()[Attrs::V[VNodeAttr::SLICEPOS]].SetLongVoid(lCount);
            vn.data()[Attrs::V[VNodeAttr::X]].SetLongVoid(lCount * NODE_X_SPACING);
            vn.data()[Attrs::V[VNodeAttr::Y]].SetLongVoid(nodeLayoutIdx * NODE_Y_SPACING);
            ++nodeLayoutIdx;
        }
        ++lCount;
        ++display;
    }

    return true;
}
