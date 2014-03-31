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

#include "mld/io/GraphImporter.h"
#include "mld/GraphTypes.h"
#include "mld/utils/Timer.h"
#include "mld/dao/MLGDao.h"
#include "mld/dao/LinkDao.h"
#include "mld/SparkseeManager.h"

using namespace mld;
using namespace sparksee::gdb;
namespace ba = boost::algorithm;

typedef std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> Converter;

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

} // end namespace anonymous

GraphImporter::GraphImporter()
{
}

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

bool GraphImporter::fromTimeSeries(sparksee::gdb::Graph* g, const std::string& nodePath,
                                   const std::string& edgePath , bool autoCreateAttributes )
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

    Converter converter; // string to wstring
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
            keyIdx[i] = converter.from_bytes(header[i]);
            if( autoCreateAttributes ) {
                if( !SparkseeManager::addAttrToNode(g, keyIdx[i], String, Indexed, Value().SetNull()) ) {
                    LOG(logERROR) << "GraphImporter::importTSNodes: failed to add attribute "
                                  << converter.to_bytes(keyIdx[i]) << " to Node";
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
            nodeData[keyIdx[i]].SetString(converter.from_bytes(tokens[i]));
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

    Converter converter; // string to wstring
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
            keyIdx[i] = converter.from_bytes(header[i]);
            if( autoCreateAttributes ) {
                if( !SparkseeManager::addAttrToHLink(g, keyIdx[i], String, Indexed, Value().SetNull()) ) {
                    LOG(logERROR) << "GraphImporter::importTSEdges: failed to add attribute "
                                  << converter.to_bytes(keyIdx[i]) << " to HLink";
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
                hlinkData[keyIdx[i]].SetStringVoid(converter.from_bytes(tokens.at(i)));
        }
        // Finally add HLink with data
        dao.addHLink(indexMap.at(src), indexMap.at(tgt), hlinkData);
    }
    infile.close();
    return true;
}
