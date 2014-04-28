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
#include "mld/model/DynamicGraph.h"
#include "mld/utils/Timer.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;

const int NODE_X_SPACING = 3;
const int NODE_Y_SPACING = 1;
//const double NODE_SIZE = 5.0;
//const std::wstring NODE_COLOR = L"#888";

using ComponentHistory = std::map<DyNodeId, bool>;

namespace {

mld::Node createDyNode( oid_t lid, oid_t nid, MLGDao* dao )
{
    OLink ol(dao->getOLink(lid, nid));
    mld::Node n(dao->getNode(nid));

    // Set Node weight
    n.setWeight(ol.weight());

    // Remove weight attribute from olink
    auto wIt = ol.data().find(Attrs::V[OLinkAttr::WEIGHT]);
    if( wIt != ol.data().end() )
        ol.data().erase(wIt);

    // Merge attribute maps
    n.data().insert(ol.data().begin(), ol.data().end());

    return n;
}

// Label community with patternId from a starting node if nodes are not in the history
void labelCommunity( DyNodeId nid, int32_t componentNum, DyGraph& g, ComponentHistory& history )
{
    DyOutNodeIter outNeighIt, outNeighEnd;

    boost::tie(outNeighIt, outNeighEnd) = boost::adjacent_vertices(nid, g);

    std::vector<DyNodeId> queue;
    // Iter through neighbors to preload for a DFS search
    for( ; outNeighIt != outNeighEnd; ++outNeighIt ) {
        auto curNeigh = *outNeighIt;
        queue.push_back(curNeigh);
        DyNode& node = g[curNeigh];
        node.data()[Attrs::V[DyNodeAttr::COMPONENTID]].SetInteger(nid);
        node.data()[Attrs::V[DyNodeAttr::COMPONENTNUM]].SetInteger(componentNum);
    }

    // DO DFS
    while( queue.size() != 0 ) {
        DyNodeId current = queue.back();
        queue.pop_back();

        // Out edges
        DyOutEdgeIter out_begin, out_end;
        for( boost::tie(out_begin, out_end) = boost::out_edges(current, g); out_begin != out_end; ++out_begin ) {
            auto curNeigh = boost::target(*out_begin, g);
            // If node not found in history
            auto it = history.find(curNeigh);
            if( it == history.end() ) {
                history[curNeigh] = true;
                queue.push_back(curNeigh);
            }
            DyNode& node = g[curNeigh];
            node.data()[Attrs::V[DyNodeAttr::COMPONENTID]].SetInteger(nid);
            node.data()[Attrs::V[DyNodeAttr::COMPONENTNUM]].SetInteger(componentNum);
        }


        // In edges
        DyInEdgeIter in_begin, in_end;
        for( boost::tie(in_begin, in_end) = boost::in_edges(current, g); in_begin != in_end; ++in_begin ) {
            auto curNeigh = boost::source(*in_begin, g);
            // If node not found in history
            auto it = history.find(curNeigh);
            if( it == history.end() ) {
                history[curNeigh] = true;
                queue.push_back(curNeigh);
            }
            // Update pattern id
            DyNode& node = g[curNeigh];
            node.data()[Attrs::V[DyNodeAttr::COMPONENTID]].SetInteger(nid);
            node.data()[Attrs::V[DyNodeAttr::COMPONENTNUM]].SetInteger(componentNum);
        }
    }
}


} // end namespace anonymous

ComponentExtractor::ComponentExtractor( Graph* g )
    : m_dao( new MLGDao(g) )
    , m_override(false)
    , m_alpha(0.0)
    , m_dg(nullptr)
{
}

ComponentExtractor::~ComponentExtractor()
{
}

bool ComponentExtractor::run()
{
    if( createDynamicGraph() ) {
        if( extractComponents() )
            return layout();
    }

    return false;
}

bool ComponentExtractor::createDynamicGraph()
{
    std::unique_ptr<Timer> t(new Timer("Extracting dynamic graph"));
    LOG(logINFO) << "Extracting dynamic graph";
    m_dg.reset( new DynamicGraph );
    if( !m_override )
        m_alpha = computeThreshold();

    auto layers = m_dao->getAllLayers();

    // Get base layer thresholded nodes and hlinks
    ObjectsPtr currentNodes(filterNodes(layers.at(0), m_alpha));
    ObjectsPtr currentHlinks(m_dao->graph()->Explode(currentNodes.get(), m_dao->hlinkType(), Outgoing));

    ProgressDisplay display(layers.size());

    ++display;

    for( size_t i = 1; i < layers.size(); ++i ) {
        ObjectsPtr nextNodes(filterNodes(layers.at(i), m_alpha));

        // Add self vlinks between nodes activated in the two layers
        ObjectsPtr commonNodes(Objects::CombineIntersection(currentNodes.get(), nextNodes.get()));
        addSelfDyEdges(layers.at(i-1), layers.at(i), commonNodes);

        // Get HLinks for next layer on thresholded nodes
        ObjectsPtr nextHlinks(m_dao->graph()->Explode(nextNodes.get(), m_dao->hlinkType(), Outgoing));
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
                    addSafeDyEdge(layers.at(i-1), src, layers.at(i), tgt);
            }

            if( currentNodes->Exists(tgt) ) {
                if( nextNodes->Exists(src) )
                    addSafeDyEdge(layers.at(i-1), tgt, layers.at(i), src);
            }
        }
        // Switch to next layer
        currentNodes = nextNodes;
        currentHlinks = nextHlinks;

        ++display;
    }

    return true;
}

void ComponentExtractor::addSelfDyEdges( const Layer& lSrc, const Layer& lTgt, const ObjectsPtr& nodes )
{
    // TODO check layer group here, do not add if not in the same group
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() ) {
        auto nid = it->Next();
        addSafeDyEdge(lSrc, nid, lTgt, nid);
    }
}

void ComponentExtractor::addSafeDyEdge( const Layer& lSrc, oid_t src, const Layer& lTgt, oid_t tgt )
{
    // TODO check layer group here, do not add if not in the same group
    // Check source
    if( !m_dg->exist(lSrc.id(), src) ) {
        m_dg->addDyNode(lSrc.id(), createDyNode(lSrc.id(), src, m_dao.get()));
    }

    // Check target
    if( !m_dg->exist(lTgt.id(), tgt) ) {
        m_dg->addDyNode(lTgt.id(), createDyNode(lTgt.id(), tgt, m_dao.get()));
    }

    //  Add edge
    m_dg->addDyEdge(lSrc.id(), src, lTgt.id(), tgt);
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

bool ComponentExtractor::extractComponents()
{
    DyGraph& g = m_dg->data();
    // while node to process in dyngraph
    // diffuse node (via DFS) in order and insert pattern Id in node data
    // do not diffuse if already flagged with a pattern

    ComponentHistory history;

    int32_t componentCount = 0;
    for( auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first ) {
        DyNodeId nid = *vp.first;
        auto it = history.find(nid);
        if( it != history.end() ) // skip if node is already in the history
            continue;

        // add in history
        history[nid] = true;
        labelCommunity(nid, componentCount, g, history);
        // inc pattern num
        componentCount++;
    }

    LOG(logDEBUG) << "ComponentExtractor::extractComponents: " << componentCount + 1;
    m_dg->setComponentCount(componentCount + 1);
    return true;
}

bool ComponentExtractor::layout()
{
    std::unique_ptr<Timer> t(new Timer("Layouting dynamic graph"));
    LOG(logINFO) << "Layouting dynamic graph";
    if( !m_dg ) {
        LOG(logERROR) << "ComponentExtractor::layout: vgraph is null";
        return false;
    }

    DyGraph& g = m_dg->data();
    GraphIndex& gIdx = m_dg->index();

    // Loop through all unique base_id to layout the y coordinate
    std::unordered_map<int64_t, int64_t> yCoordMap;
    Layer base(m_dao->baseLayer());
    ObjectsPtr ids(m_dao->getAllNodeIds(base));

    auto it = ids->Iterator();
    int64_t y = 0;
    while( it->HasNext() ) {
        int64_t nid = static_cast<int64_t>(it->Next());
        yCoordMap[nid] = y++;
    }

    ProgressDisplay display(gIdx.size());
    // Loop through all layers to set the rest of the properties
    uint32_t lCount = 0;
    for( auto& lp: m_dg->layerMap() ) {  // iterate through layer in correct order
        for( auto& vnPair: gIdx.at(lp.first) ) { // for each layer in index
            DyNode& vn = g[vnPair.second]; // get bundled property
            // override id with VGraph NodeId
            vn.setId(static_cast<oid_t>(vnPair.second));
            vn.data()[Attrs::V[DyNodeAttr::LAYERPOS]].SetLongVoid(lp.second);
            vn.data()[Attrs::V[DyNodeAttr::SLICEPOS]].SetLongVoid(lCount);
            vn.data()[Attrs::V[DyNodeAttr::X]].SetLongVoid(lCount * NODE_X_SPACING);
            int64_t baseId = vn.data()[Attrs::V[DyNodeAttr::BASEID]].GetLong();
            vn.data()[Attrs::V[DyNodeAttr::Y]].SetLongVoid(yCoordMap.at(baseId) * NODE_Y_SPACING);

            // vn.data()[Attrs::V[VNodeAttr::SIZE]].SetDoubleVoid(NODE_SIZE);
            // vn.data()[Attrs::V[VNodeAttr::COLOR]].SetStringVoid(NODE_COLOR);
        }
        ++lCount;
        ++display;
    }

    return true;
}
