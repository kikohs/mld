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
#include <boost/graph/connected_components.hpp>

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

} // end namespace anonymous

ComponentExtractor::ComponentExtractor( Graph* g )
    : m_dao( new MLGDao(g) )
    , m_override(false)
    , m_alphas()
    , m_dg(nullptr)
{
}

ComponentExtractor::~ComponentExtractor()
{
}

bool ComponentExtractor::run()
{
    if( createDynamicGraph() ) {
        if( extractComponents() ) {
            if( layout() ) {
                LOG(logINFO) << *m_dg;
                return true;
            }
        }
    }

    return false;
}

bool ComponentExtractor::createDynamicGraph()
{
    std::unique_ptr<Timer> t(new Timer("Extracting dynamic graph"));
    LOG(logINFO) << "Extracting dynamic graph";
    m_dg.reset( new DynamicGraph );
    std::vector<TSGroupId> groups(m_dao->getAllTSGroupIds());
    m_alphas = computeThresholds(groups);
    m_dg->setGroupCount(groups.size());

    LOG(logINFO) << "Computed thresholds";
    for( auto v: m_alphas ) {
        LOG(logINFO) << v;
    }

    LOG(logINFO) << "Start extracting active layers";
    auto layers = m_dao->getAllLayers();

    ProgressDisplay display(layers.size() * groups.size());
    // For each group
    for( size_t j = 0; j < groups.size(); ++j ) {
        // LOG(logDEBUG) << "Group: " << groups.at(j) << " alpha: " << m_alphas.at(j);
        ObjectsPtr currentNodes(filterNodes(layers.at(0), groups.at(j), m_alphas.at(j)));
        ObjectsPtr currentHlinks(m_dao->graph()->Explode(currentNodes.get(), m_dao->hlinkType(), Outgoing));
        ++display;
        // For each layer filtered by group
        for( size_t i = 1; i < layers.size(); ++i ) {
            ObjectsPtr nextNodes(filterNodes(layers.at(i), groups.at(j), m_alphas.at(j)));
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
    }

    return true;
}

void ComponentExtractor::addSelfDyEdges( const Layer& lSrc, const Layer& lTgt, const ObjectsPtr& nodes )
{
    ObjectsIt it(nodes->Iterator());
    while( it->HasNext() ) {
        auto nid = it->Next();
        addSafeDyEdge(lSrc, nid, lTgt, nid);
    }
}

void ComponentExtractor::addSafeDyEdge( const Layer& lSrc, oid_t src, const Layer& lTgt, oid_t tgt )
{
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

std::vector<double> ComponentExtractor::computeThresholds( const std::vector<TSGroupId>& groups )
{
    std::vector<double> res;
    auto olWeightAttr = m_dao->graph()->FindAttribute(m_dao->olinkType(), Attrs::V[OLinkAttr::WEIGHT]);
    // no groups
    if( groups.size() == 1 ) {
        if( m_override ) {
            res.push_back(m_alphaOverride);
            return res;
        }

        std::unique_ptr<AttributeStatistics> stats(m_dao->graph()->GetAttributeStatistics(olWeightAttr, true));
        double maxVal = stats->GetMax().GetDouble();
        double minVal = stats->GetMin().GetDouble();

        // 0.5 * max(abs(X))
        res.push_back(std::max(std::abs(maxVal), std::abs(minVal)) / 2.0);
        return res;
    }

    // Groups of values
    Value val;
    for( auto& gId: groups ) {
        if( m_override ) {
            res.push_back(m_alphaOverride);
        }
        else {
            double minVal = 0.0;
            double maxVal = 0.0;
            ObjectsPtr olinks(m_dao->getOLinksByTSGroup(gId));
            ObjectsIt it(olinks->Iterator());
            while( it->HasNext() ) {
                oid_t ol = it->Next();
                m_dao->graph()->GetAttribute(ol, olWeightAttr, val);
                double v = val.GetDouble();

                // Get both max and min value
                if( v > maxVal )
                    maxVal = v;
                if( v < minVal )
                    minVal = v;
            }

            // Get threshold for this group
            res.push_back(std::max(std::abs(maxVal), std::abs(minVal)) / 2.0);
        }
    }

    return res;
}

ObjectsPtr ComponentExtractor::filterNodes( const Layer& layer, TSGroupId group, double threshold )
{
    ObjectsPtr olinks(m_dao->graph()->Explode(layer.id(), m_dao->olinkType(), Outgoing));

    ObjectsPtr olGroup(m_dao->getOLinksByTSGroup(group));
    ObjectsPtr layerGroup(Objects::CombineIntersection(olinks.get(), olGroup.get()));
    Value v;
    v.SetDoubleVoid(threshold);
    // Select on restricted edge set
    auto olWeightAttr = m_dao->graph()->FindAttribute(m_dao->olinkType(), Attrs::V[OLinkAttr::WEIGHT]);
    ObjectsPtr filterOl(m_dao->graph()->Select(olWeightAttr, GreaterEqual, v, layerGroup.get()));
    // Get olinks with value x <= -threshold
    v.SetDoubleVoid(-threshold);
    ObjectsPtr filterOl2(m_dao->graph()->Select(olWeightAttr, LessEqual, v, layerGroup.get()));
    filterOl->Union(filterOl2.get());
    // fill nodes to return
    return ObjectsPtr(m_dao->graph()->Heads(filterOl.get()));
}

bool ComponentExtractor::extractComponents()
{
    DyGraph& g = m_dg->data();

    std::vector<int> component(boost::num_vertices(g));
    size_t componentCount = boost::connected_components(g, &component[0]);
    int i = 0;
    for( auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first ) {
        DyNodeId nid = *vp.first;
        g[nid].data()[Attrs::V[DyNodeAttr::COMPONENTNUM]].SetInteger(component.at(i));
        ++i;
    }
    m_dg->setComponentCount(componentCount);
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
            int64_t x = lCount * NODE_X_SPACING;
            vn.data()[Attrs::V[DyNodeAttr::X]].SetLongVoid(x);
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
