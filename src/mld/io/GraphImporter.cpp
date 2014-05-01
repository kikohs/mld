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

#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <functional>
#include <boost/bind.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <sparksee/gdb/Objects.h>

#ifdef MLD_USE_JSON
    #include <json_spirit.h>
#endif

#include "mld/io/GraphImporter.h"
#include "mld/GraphTypes.h"
#include "mld/utils/Timer.h"
#include "mld/dao/MLGDao.h"
#include "mld/dao/LinkDao.h"
#include "mld/SparkseeManager.h"

using namespace mld;
using namespace sparksee::gdb;
namespace ba = boost::algorithm;

#ifdef MLD_USE_JSON
namespace js = json_spirit;

struct AugmentedGraph {
    int tsCount;
    int tsDataSize;
    js::wmArray nodes;
    js::wmArray edges;
    js::wmArray ts;
};

#endif

typedef std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> Converter;
static Converter gConverter; // string to wstring

namespace {

oid_t addOrGetFromIndexMap( uint64_t node, GraphImporter::IndexMap& indexMap,
                            const Layer& base, MLGDao& dao )
{
    oid_t id = 0;
    auto it = indexMap.find(node);
    if( it != indexMap.end() ) {
        id = it->second;
    }
    else {  // Not in map
        // Add to graph
        std::wstringstream stream;
        stream << node;
        AttrMap data;
        data[Attrs::V[NodeAttr::LABEL]].SetStringVoid(stream.str());
        mld::Node n(dao.addNodeToLayer(base, data));
        indexMap[node] = n.id();
        id = n.id();
    }
    return id;
}

std::vector<std::string> getNextLineAndSplitIntoTokens( std::istream& str )
{
    std::vector<std::string> result;
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    while( std::getline(lineStream, cell, ',') ) {
        cell.erase(boost::remove_if(cell, ba::is_any_of("#\"\r")), cell.end());
        result.push_back(cell);
    }
    return result;
}

#ifdef MLD_USE_JSON

bool findValue( js::wmValue& out,
                const std::wstring& name,
                const js::wmObject& obj )
{
    auto it = obj.find(name);

    if( it == obj.end() )
        return false;

    out = it->second;
    return true;
}


AttrMap createNodeData( const js::wmValue& n )
{
    AttrMap res;
    const js::wmObject& node = n.get_obj();

    for( auto kv: node ) {
        if( kv.first == L"label" ) {
            res[Attrs::V[NodeAttr::LABEL]].SetStringVoid(kv.second.get_str());
        }
        else if( kv.first == L"weight" ) {
            res[Attrs::V[NodeAttr::WEIGHT]].SetDoubleVoid(kv.second.get_real());
        }
        else if( kv.first == L"id" ) {
            res[L"id"].SetStringVoid(std::to_wstring(kv.second.get_int()));
        }
        else {
            res[kv.first].SetStringVoid(kv.second.get_str());
        }
    }
    return res;
}

bool parseAugmentedGraph( AugmentedGraph& ag, const std::string& filepath )
{
    std::wifstream in(filepath);

    js::wmValue root;
    if( !js::read(in, root) ) {
        LOG(logERROR) << "GraphImporter::parseAugmentedGraph error parsing json";
        return false;
    }

    js::wmObject& begin(root.get_obj());
    js::wmValue k;

    if( !findValue(k, L"nodes", begin) ) {
        LOG(logERROR) << "GraphImporter::parseAugmentedGraph cannot read: key:nodes";
        return false;
    }
    ag.nodes = k.get_array();

    if( !findValue(k, L"links", begin) ) {
        LOG(logERROR) << "GraphImporter::parseAugmentedGraph cannot read: key:edges";
        return false;
    }

    ag.edges = k.get_array();

    if( !findValue(k, L"graph", begin) ) {
        LOG(logERROR) << "GraphImporter::parseAugmentedGraph cannot read: key:graph";
        return false;
    }

    js::wmArray& graphArray = k.get_array();
    if( graphArray.size() < 1 ) {
        LOG(logERROR) << "GraphImporter::parseAugmentedGraph graph top array is empty";
        return false;
    }
    // The first value is also an array
    js::wmArray& graphDataArray = graphArray.at(0).get_array();

    // Graph data are embedded in array, where the first value is 'graph_data'
    // and the second one the actual payload with all the data
    if( graphDataArray.size() < 2 ) {
        LOG(logERROR) << "GraphImporter::fromTimeSeriesJson graph data array is empty";
        return false;
    }
    js::wmObject& graphData = graphDataArray.at(1).get_obj();

    js::wmValue tsV;
    if( !findValue(tsV, L"ts_count", graphData) ) {
        LOG(logERROR) << "GraphImporter::fromTimeSeriesJson cannot read: key:ts_count";
        return false;
    }
    ag.tsCount = tsV.get_int();

    if( !findValue(tsV, L"ts_data_size", graphData) ) {
        LOG(logERROR) << "GraphImporter::fromTimeSeriesJson cannot read: key:ts_size";
        return false;
    }

    ag.tsDataSize = tsV.get_int();

    if( !findValue(tsV, L"ts", graphData) ) {
        LOG(logERROR) << "GraphImporter::fromTimeSeriesJson cannot read: key:ts";
        return false;
    }

    ag.ts = tsV.get_array();

    in.close();
    return true;
}

#endif

} // end namespace anonymous

bool GraphImporter::fromSnapFormat( Graph* g, const std::string& filepath )
{
    std::unique_ptr<Timer> t(new Timer("Importing snap graph"));
    LOG(logINFO) << "Parsing SNAP undirected graph: " << filepath;
    MLGDao dao(g);
    LinkDao linkDao(g);
    Layer base = dao.addBaseLayer();
    if( base.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "GraphImporter::fromSnapFormat cannot add base layer";
        return false;
    }

    std::ifstream infile(filepath.c_str());
    if( !infile ) {
        LOG(logERROR) << "GraphImporter::fromSnapFormat cannot open file " << filepath;
        return false;
    }

    std::string read_line;
    // Skip comments
    while( std::getline(infile, read_line) ) {
        if( read_line[0] != '#' )
            break;
    }
    // Go backwards one line
    long pos = infile.tellg();
    long startPos = pos - (read_line.size() + 1);
    infile.seekg(startPos);

    IndexMap indexMap;
    uint64_t src;
    uint64_t tgt;
    while( infile >> src >> tgt ) {
        if( src == tgt ) // Skip self loop
            continue;
        // Add node if needed
        oid_t srcId = addOrGetFromIndexMap(src, indexMap, base, dao);
        oid_t tgtId = addOrGetFromIndexMap(tgt, indexMap, base, dao);
        // Create HLink
        linkDao.addHLink(srcId, tgtId);
    }
    infile.close();

    LOG(logINFO) << "Base Layer #nodes: " << dao.getNodeCount(base) << " #edges: " << dao.getHLinkCount(base);
    return true;
}

bool GraphImporter::fromTimeSeries( sparksee::gdb::Graph* g, const std::string& nodePath,
                                   const std::string& edgePath, bool autoCreateAttributes )
{
    std::unique_ptr<Timer> t(new Timer("Importing TimeSeries graph"));
    IndexMap indexMap;
    if( !importTSNodes(g, nodePath, indexMap, autoCreateAttributes) ) {
        LOG(logERROR) << "GraphImporter::fromTimeSeries error importing nodes data: " << nodePath;
        return false;
    }
    if( !importTSEdges(g, edgePath, indexMap, autoCreateAttributes) ) {
        LOG(logERROR) << "GraphImporter::fromTimeSeries error importing edges data: " << edgePath;
        return false;
    }

    MLGDao d(g);
    Layer base(d.baseLayer());
    LOG(logINFO) << "Base Layer #nodes: " << d.getNodeCount(base)
                 << " #edges: " << d.getHLinkCount(base)
                 << " #timeseries: " << d.getLayerCount();

    return true;
}

bool GraphImporter::importTSNodes( sparksee::gdb::Graph* g,
                                   const std::string& nodePath,
                                   IndexMap& indexMap, bool autoCreateAttributes )
{
    LOG(logINFO) << "Parsing node data: " << nodePath;

    std::ifstream infile(nodePath.c_str());
    if( !infile ) {
        LOG(logERROR) << "GraphImporter::importTSNodes cannot open file " << nodePath;
        return false;
    }

    auto header = getNextLineAndSplitIntoTokens(infile);
    std::vector<std::string> tsTok;
    ba::split(tsTok, header.back(), ba::is_any_of(":"));
    if( tsTok[0] != "ts" || tsTok.size() != 2 ) {
        LOG(logERROR) << std::string("GraphImporter::importTSNodes key ts (timeseries) not ") +
                         std::string("found in header or not in the last position: ") << header.back();
        return false;
    }
    // Remove ts value for header
    header.pop_back();
    // Get ts series size
    size_t tsSize = size_t(std::stoi(tsTok[1]));
    size_t tsStartIdx = header.size();

    // Create an data template with keys read from the header
    std::map<int, std::wstring> keyIdx;  // store indx to keys for AttrMap
    AttrMap model;
    for( size_t i = 0; i < header.size(); ++i ) {
        ba::to_lower(header[i]);
        if( header[i] == "weight" ) {
            keyIdx[i] = Attrs::V[NodeAttr::WEIGHT];
        }
        else if( header[i] == "label" ) {
            keyIdx[i] = Attrs::V[NodeAttr::LABEL];
        }
        else {
            keyIdx[i] = gConverter.from_bytes(header[i]);
            if( autoCreateAttributes ) {
                if( !SparkseeManager::addAttrToNode(g, keyIdx[i], String, Indexed, Value().SetNull()) ) {
                    LOG(logERROR) << "GraphImporter::importTSNodes: failed to add attribute "
                                  << gConverter.to_bytes(keyIdx[i]) << " to Node";
                }
            }
        }
        model[keyIdx[i]].SetNullVoid();
    }

    // Create layer stack
    MLGDao dao(g);
    Layer base = dao.addBaseLayer();
    std::vector<Layer> layerStack;
    layerStack.reserve(tsSize);
    layerStack.push_back(base);

    // Create layer stack to store TS values on OLink
    for( size_t i = 1; i < tsSize; ++i ) {
        layerStack.push_back(dao.addLayerOnTop());
    }

    uint64_t k = 0; // Count node for indexMap
    // Read lines here
    while( infile.good() ) {
        auto tokens(getNextLineAndSplitIntoTokens(infile));
        if( tokens.empty() )
            break;
        // Copy header model keys
        AttrMap nodeData(model);
        // Set values
        for( size_t i = 0; i < header.size(); ++i ) {
            nodeData[keyIdx[i]].SetString(gConverter.from_bytes(tokens[i]));
        }

        // Set OLink data for first edge
        AttrMap olinkData;
        olinkData[Attrs::V[OLinkAttr::WEIGHT]].SetDouble(std::stod(tokens[tsStartIdx]));

        Node node(dao.addNodeToLayer(base, nodeData, olinkData));
        indexMap[k++] = node.id();
        // Add olink to other layers
        for( size_t i = tsStartIdx + 1; i < tokens.size(); ++i ) {
            olinkData[Attrs::V[OLinkAttr::WEIGHT]].SetDouble(std::stod(tokens[i]));
            dao.addOLink(layerStack[i - tsStartIdx], node, olinkData);
        }
    }
    infile.close();
    return true;
}

bool GraphImporter::importTSEdges( sparksee::gdb::Graph* g,
                                   const std::string& edgePath,
                                   const IndexMap& indexMap,
                                   bool autoCreateAttributes )
{
    LOG(logINFO) << "Parsing edge data: " << edgePath;
    std::ifstream infile(edgePath);
    if( !infile ) {
        LOG(logERROR) << "GraphImporter::importTSEdges cannot open file " << edgePath;
        return false;
    }

    if( indexMap.empty() ) {
        LOG(logERROR) << "GraphImporter::importTSEdges empty indexMap";
        return false;
    }

    auto header = getNextLineAndSplitIntoTokens(infile);
    if( header.size() < 2 ) {
        LOG(logERROR) <<  "GraphImporter::importTSEdges error in parsing source and target id";
        return false;
    }

    // Create an data template with keys read from the header
    std::map<int, std::wstring> keyIdx;  // store indx to keys for AttrMap
    AttrMap model;
    int weightIdx = -1;
    for( size_t i = 2; i < header.size(); ++i ) {
        ba::to_lower(header[i]);
        if( header[i] == "weight" ) {
            keyIdx[i] = Attrs::V[OLinkAttr::WEIGHT];
            weightIdx = int(i); // save weight index if any
        }
        else {
            keyIdx[i] = gConverter.from_bytes(header[i]);
            if( autoCreateAttributes ) {
                if( !SparkseeManager::addAttrToHLink(g, keyIdx[i], String, Indexed, Value().SetNull()) ) {
                    LOG(logERROR) << "GraphImporter::importTSEdges: failed to add attribute "
                                  << gConverter.to_bytes(keyIdx[i]) << " to HLink";
                }
            }
        }
        model[keyIdx[i]].SetNullVoid();
    }

    LinkDao dao(g);
    // Read lines here
    while( infile.good() ) {
        auto tokens(getNextLineAndSplitIntoTokens(infile));
        if( tokens.empty() || tokens.size() < 2 )
            break;

        uint64_t src = std::stoull(tokens[0]);
        uint64_t tgt = std::stoull(tokens[1]);

        if( src == tgt ) // Skip self loop
            continue;

        AttrMap hlinkData(model);

        // Skip src and tgt, save weight to double is there is a weight
        for( int i = 2; i < int(tokens.size()); ++i ) {
            if( i == weightIdx ) // Save weight
                hlinkData[keyIdx[i]].SetDoubleVoid(std::stod(tokens.at(i)));
            else
                hlinkData[keyIdx[i]].SetStringVoid(gConverter.from_bytes(tokens.at(i)));
        }
        // Finally add HLink with data
        dao.addHLink(indexMap.at(src), indexMap.at(tgt), hlinkData);
    }
    infile.close();
    return true;
}

bool GraphImporter::fromTimeSeriesJson( Graph* g, const std::string& filename, bool autoCreateAttributes )
{
#ifndef MLD_USE_JSON
    LOG(logERROR) << "GraphImporter::fromTimeSeriesJson MLD_USE_JSON not defined in the project options";
    return false;
#endif
    std::unique_ptr<Timer> t(new Timer("Importing TimeSeries as json"));
    LOG(logINFO) << "Importing TimeSeries graph dataset from json" << filename;

    if( !g ) {
        LOG(logERROR) << "GraphImporter::fromTimeSeriesJson graph is null";
        return false;
    }

    AugmentedGraph ag;
    if( !parseAugmentedGraph(ag, filename) ) {
        LOG(logERROR) << "GraphImporter::fromTimeSeriesJson: parsing graph failed";
        return false;
    }

    // Create layers
    MLGDao dao(g);
    Layer base(dao.addBaseLayer());

    std::vector<Layer> layers;
    layers.reserve(ag.tsDataSize);
    layers.push_back(base);

    // Create all layers
    for( int i = 1; i < ag.tsDataSize; ++i )
        layers.push_back(dao.addLayerOnTop());

    // Create node attributes if needed
    if( autoCreateAttributes ) {
        AttrMap nodeAttr(createNodeData(ag.nodes.at(0)));
        for( auto kv: nodeAttr ) {

            auto it = std::find_if(Attrs::V.begin(), Attrs::V.end(),
                                   boost::bind(&Enum2Wstring::value_type::second, _1) == kv.first);
            // if node key is not already in the base scheme map
            if( it == Attrs::V.end() ) {
                LOG(logINFO) << "Creating attribute: " << gConverter.to_bytes(kv.first);
                if( !SparkseeManager::addAttrToNode(g, kv.first, String, Indexed, Value().SetNull()) ) {
                    LOG(logERROR) << "GraphImporter::fromTimeSeriesJson: failed to add attribute "
                                  << gConverter.to_bytes(kv.first) << " to Node";
                }
            }
        }
    }

    // Map input ip, ts id, node oid
    std::unordered_map<size_t, std::unordered_map<int, Node>> multiIndexMap;

    // Create nodes
    LOG(logINFO) << "Creating nodes";
    for( size_t i = 0; i < ag.nodes.size(); ++i ) {
        AttrMap nodeAttr(createNodeData(ag.nodes.at(i)));
        for( int j = 0; j < ag.tsCount; ++j ) {  // for each timeseries
            nodeAttr[Attrs::V[NodeAttr::TSID]].SetInteger(j);
            Node n(dao.addNodeToLayer(base, nodeAttr));
            multiIndexMap[i][j] = n;
        }
    }

    // Create edges
    LOG(logINFO) << "Creating edges";
    for( size_t i = 0; i < ag.edges.size(); ++i ) {
        const js::wmObject& edge = ag.edges.at(i).get_obj();

        size_t source = edge.at(L"source").get_int();
        size_t target = edge.at(L"target").get_int();
        auto it = edge.find(L"weight");

        // Add weight if any
        AttrMap data;
        if( it != edge.end() ) {
           double weight = it->second.get_real();
           data[Attrs::V[HLinkAttr::WEIGHT]].SetDoubleVoid(weight);
        }

        // for each timeseries
        for( int j = 0; j < ag.tsCount; ++j ) {
            Node& src(multiIndexMap.at(source).at(j));
            Node& tgt(multiIndexMap.at(target).at(j));
            dao.addHLink(src, tgt, data);  // add hlink
        }
    }

    // Add timeseries data
    LOG(logINFO) << "Add timeseries data";
    // For each timeseries series ..
    for( size_t j = 0; j < ag.ts.size(); ++j ) {
        const js::wmArray& tsData(ag.ts.at(j).get_array());
        for( size_t i = 0; i < ag.nodes.size(); ++i ) {  // for each node
            const js::wmArray& tsDataForNode(tsData.at(i).get_array());
            for( size_t k = 0; k < tsDataForNode.size(); ++k ) {  // for each array of value per node
                double olWeight = tsDataForNode.at(k).get_real();
                if( k == 0 ) { // update existing olink
                    OLink ol = dao.getOLink(layers.at(k).id(), multiIndexMap.at(i).at(j).id());
                    ol.setWeight(olWeight);
                    dao.updateOLink(ol);
                }
                else {  // Add new olinks
                    AttrMap data;
                    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(olWeight);
                    dao.addOLink(layers.at(k), multiIndexMap.at(i).at(j), data);
                }
            }
        }
    }

    return true;
}
