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

#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <sparksee/gdb/Objects.h>

#ifdef MLD_USE_JSON
    #include <json_spirit.h>
#endif

#include "mld/io/GraphExporter.h"
#include "mld/GraphTypes.h"
#include "mld/dao/MLGDao.h"
#include "mld/dao/LinkDao.h"
#include "mld/SparkseeManager.h"
#include "mld/model/DynamicGraph.h"

#include "mld/utils/Timer.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;
namespace ba = boost::algorithm;
#ifdef MLD_USE_JSON
    namespace js = json_spirit;
    const int DOUBLE_PRECISION = 8;
#endif

using Converter = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>;
Converter gConverter;

namespace {

std::wstring convertValueToWString( const Value& v )
{
    switch( v.GetDataType() ) {
        case Integer:
            return std::to_wstring(v.GetInteger());
        case Long:
            return std::to_wstring(v.GetLong());
        case String:
            return v.GetString();
        case Double:
            return std::to_wstring(v.GetDouble());
        case Boolean:
            return std::to_wstring(v.GetBoolean());
        case OID:
            return std::to_wstring(v.GetOID());
        default:
            LOG(logERROR) << "GraphExporter::convertValueToWString unsupported datatype";
            break;
    }
    return std::wstring();
}

#ifdef MLD_USE_JSON

void writeDyNode( js::wmArray& nodes, const mld::Node& n )
{
    js::wmObject nodeObj;
    auto it = n.data().find(L"id");
    if( it == n.data().end() ) {
        // nodeObj[L"id"] = n.id();
        // Should convert as string for sigma js
        nodeObj[L"id"] = std::to_wstring(n.id());
    }

    for( auto& kv: n.data() ) {
        if( kv.first == L"id" ) {
            nodeObj[L"id"] = kv.second.GetString();
        }
        else if( kv.first == Attrs::V[NodeAttr::LABEL] ) {
            std::wstring label(kv.second.GetString());
            if( label != L"" )
                nodeObj[L"label"] = label;
            else
                nodeObj[L"label"] = std::to_wstring(n.id());
        }
        else if( kv.first == Attrs::V[NodeAttr::WEIGHT] ) {
            nodeObj[L"weight"] = kv.second.GetDouble();
        }
        else if( kv.first == Attrs::V[NodeAttr::TSID] ) {
            nodeObj[L"ts_group_id"] = kv.second.GetInteger();
        }
        else if( kv.first == Attrs::V[DyNodeAttr::LAYERID] ) {
            // nodeObj[L"layer_id"] = kv.second.GetLong();
            ;  // do not export
        }
        else if( kv.first == Attrs::V[DyNodeAttr::BASEID] ) {
            // nodeObj[L"base_id"] = kv.second.GetLong();
            ;  // do not export
        }
        else if( kv.first == Attrs::V[DyNodeAttr::LAYERPOS] ) {
            nodeObj[L"layer_pos"] = kv.second.GetLong();
        }
        else if( kv.first == Attrs::V[DyNodeAttr::SLICEPOS] ) {
            nodeObj[L"slice_pos"] = kv.second.GetLong();
        }
        else if( kv.first == Attrs::V[DyNodeAttr::X] ) {
            nodeObj[L"x"] = kv.second.GetLong();
        }
        else if( kv.first == Attrs::V[DyNodeAttr::Y] ) {
            nodeObj[L"y"] = kv.second.GetLong();
        }
//        else if( kv.first == Attrs::V[VNodeAttr::SIZE] ) {
//            nodeObj[L"size"] = kv.second.GetDouble();
//        }
//        else if( kv.first == Attrs::V[VNodeAttr::COLOR] ) {
//            nodeObj[L"color"] = kv.second.GetString();
//        }
        else if( kv.first == Attrs::V[DyNodeAttr::INPUTID] ) {
            nodeObj[L"input_id"] = kv.second.GetInteger();
        }
        else if( kv.first == Attrs::V[DyNodeAttr::COMPONENTID] ) {
            nodeObj[L"component_id"] = kv.second.GetInteger();
        }
        else if( kv.first == Attrs::V[DyNodeAttr::COMPONENTNUM] ) {
            nodeObj[L"component_num"] = kv.second.GetInteger();
        }
        else {
            nodeObj[kv.first] = convertValueToWString(kv.second);
        }
    }

    // Append to the list of nodes
    nodes.push_back(nodeObj);
}

void writeDyEdge( js::wmArray& edges, const uint32_t eid, const DyNodeId src, const DyNodeId tgt )
{
    js::wmObject edgeObj;
    edgeObj[L"source"] = std::to_wstring(static_cast<int>(src));
    edgeObj[L"target"] = std::to_wstring(static_cast<int>(tgt));
    edgeObj[L"id"] = std::to_wstring(static_cast<int>(eid));
    // Append to the list of edges
    edges.push_back(edgeObj);
}


#endif

void writeWStringToFile( std::wofstream& out, const std::wstring v, bool closeLine=false )
{
    out << v;
    if( closeLine )
        out << L"\n";
    else
        out << ",";
}

} // end namespace anonymous

bool GraphExporter::toTimeSeries( Graph* g, const std::string& name, std::string& exportFolderPath )
{
    std::unique_ptr<Timer> t(new Timer("Exporting TimeSeries graph"));
    RIndexMap indexMap;
    if( !ba::ends_with(exportFolderPath, "/") ) {
        exportFolderPath.append("/");
    }
    std::string nodePath = exportFolderPath + name + ".nodes.csv";

    MLGDao dao(g);

    if( writeTSNodes(dao, nodePath, indexMap) ) {
        LOG(logINFO) << "Wrote: " << nodePath;
    }
    else {
        LOG(logERROR) << "GraphExporter::toTimeSeries error writing node data to: " << nodePath;
        return false;
    }

    std::string edgePath = exportFolderPath + name + ".edges.csv";
    if( writeTSEdges(dao, edgePath, indexMap) ) {
        LOG(logINFO) << "Wrote: " << edgePath;
    }
    else {
        LOG(logERROR) << "GraphImporter::fromTimeSeries error writing edge data to: " << edgePath;
        return false;
    }

    return true;
}

bool GraphExporter::writeTSNodes( MLGDao& dao, const std::string& nodePath, RIndexMap& indexMap )
{
    LOG(logINFO) << "Start writing nodes";

    std::wofstream outfile(nodePath);
    if( !outfile ) {
        LOG(logERROR) << "GraphExporter::writeTSNodes cannot open file " << nodePath;
        return false;
    }

    Layer base(dao.baseLayer());
    NodeVec nodes(dao.getAllNodes(base));

    if( nodes.empty() ) {
        LOG(logERROR) << "GraphExporter::writeTSNodes no nodes in graph";
        return false;
    }
    Node n1(nodes.front());

    // Create header with id field in first position
    bool hasIdField = false;
    outfile << L"#id,";
    for( auto& kv: n1.data() ) {
        if( kv.first == L"id" ) {
            hasIdField = true;
        }
        else if( kv.first == Attrs::V[NodeAttr::LABEL] ) {
            writeWStringToFile(outfile, L"label");
        }
        else if( kv.first == Attrs::V[NodeAttr::WEIGHT] ) {
            writeWStringToFile(outfile, L"weight");
        }
        else {
            writeWStringToFile(outfile, kv.first);
        }
    }
    // Write header TS
    auto lCount = dao.getLayerCount();
    outfile << "ts:" << lCount << L"\n";

    ProgressDisplay display(nodes.size());

    // Write data
    for( size_t i = 0; i < nodes.size(); ++i ) {
        Node n = nodes.at(i);
        AttrMap data(n.data());
        std::wstring id;
        if( hasIdField ) {
            id = convertValueToWString(n.data()[L"id"]);
            data.erase(data.find(L"id"));
        }
        else {
            id = std::to_wstring(i);
        }
        writeWStringToFile(outfile, id);
        indexMap[n.id()] = id;

        // Key id has be removed if it was present
        for( auto& kv: data ) { // iterate through key and write value to file
            writeWStringToFile(outfile, convertValueToWString(kv.second));
        }

        // Write TS data
        outfile << L"\"";
        auto olinks( dao.getAllOLinks(n.id()));
        for( size_t i = 0; i < olinks.size() - 1; ++i ) {
            outfile << olinks.at(i).weight() << L",";
        }
        // Write last value
        outfile << olinks.at(i).weight();
        outfile << L"\"";
        outfile << L"\n";
        ++display;
    }

    outfile.close();
    return true;
}

bool GraphExporter::writeTSEdges( MLGDao& dao, const std::string& edgePath, RIndexMap& indexMap )
{
    LOG(logINFO) << "Start writing edges";

    std::wofstream outfile(edgePath);
    if( !outfile ) {
        LOG(logERROR) << "GraphExporter::writeTSEdges cannot open file " << edgePath;
        return false;
    }

    Layer base(dao.baseLayer());
    auto hlinks = dao.getAllHLinks(base);

    HLink hl1(hlinks.front());
    // Create header
    outfile << L"#source,target,";
    size_t i = 0;
    size_t dataSize = hl1.data().size();
    for( auto& kv: hl1.data() ) {
        std::wstring v;
        if( kv.first == Attrs::V[HLinkAttr::WEIGHT] )
            v = L"weight";
        else
            v = kv.first;

        if( i < dataSize - 1 )
            writeWStringToFile(outfile, v);
        else
            writeWStringToFile(outfile, v, true);
        ++i;
    }

    ProgressDisplay display(hlinks.size());
    // Write data
    for( auto& hl: hlinks ) {
        // Src and target
        auto src = indexMap.at(hl.source());
        auto tgt = indexMap.at(hl.target());
        writeWStringToFile(outfile, src);
        writeWStringToFile(outfile, tgt);

        // Write HLink data
        size_t i = 0;
        size_t dataSize = hl.data().size();
        for( auto& kv: hl.data() ) {
            if( i < dataSize - 1 )
                writeWStringToFile(outfile, convertValueToWString(kv.second));
            else
                writeWStringToFile(outfile, convertValueToWString(kv.second), true);
            ++i;
        }
        ++display;
    }

    return true;
}

bool GraphExporter::exportDynamicGraphAsJson( const DynGraphPtr& dyGraph, const std::string& filename )
{
#ifndef MLD_USE_JSON
    LOG(logERROR) << "GraphExporter::exportDynamicGraphAsJson:"
                     << "MLD_USE_JSON is not defined, please activate it via the project options";
    return false;
#endif

    std::unique_ptr<Timer> t(new Timer("Exporting Dynamic Graph as json"));
    LOG(logINFO) << "Exporting Dynamic as json to: " << filename;

    if( !dyGraph ) {
        LOG(logERROR) << "GraphExporter::exportDynamicGraphAsJson dynGraph is null";
        return false;
    }

    std::wofstream out(filename);

    DyGraph& g = dyGraph->data();
    auto numVertices = boost::num_vertices(g);
    auto numEdges = boost::num_edges(g);

    ProgressDisplay display(numVertices + numEdges + 1); // account for write file

    js::wmObject graphObj;

    // General properties
    graphObj[L"node_count"] = static_cast<int64_t>(numVertices);
    graphObj[L"edge_count"] = static_cast<int64_t>(numEdges);
    graphObj[L"layer_count"] = static_cast<int64_t>(dyGraph->layerMap().size());
    graphObj[L"component_count"] = static_cast<int32_t>(dyGraph->componentCount());

    // Nodes
    js::wmArray nodes;
    // Iterate through nodes and serialize them into nodesObj
    const DyIndexMap& vindex = boost::get(boost::vertex_index, g);

    for( auto vp = boost::vertices(g); vp.first != vp.second; ++vp.first ) {
        writeDyNode(nodes, g[vindex[*vp.first]]);
        ++display;
    }
    graphObj[L"nodes"] = nodes;

    // Edges
    js::wmArray edges;
    DyEdgeIter ei, ei_end;
    uint32_t edgeIdx = 0;
    for( boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei ) {
        writeDyEdge(edges, edgeIdx, vindex[boost::source(*ei, g)], vindex[boost::target(*ei, g)]);
        ++edgeIdx;
        ++display;
    }
    graphObj[L"edges"] = edges;

    js::write(graphObj, out, js::none, DOUBLE_PRECISION);
    // js::write_formatted(graphObj, out, DOUBLE_PRECISION);
    ++display;
    out.close();

    return true;
}
