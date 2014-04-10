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

#include "mld/io/GraphImporter.h"
#include "mld/GraphTypes.h"
#include "mld/utils/Timer.h"
#include "mld/dao/MLGDao.h"
#include "mld/dao/LinkDao.h"
#include "mld/SparkseeManager.h"

#include "mld/io/GraphExporter.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;
namespace ba = boost::algorithm;
#ifdef MLD_USE_JSON
    namespace js = json_spirit;
#endif

using Converter = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>;
Converter converter;

namespace {

#ifdef MLD_USE_JSON
//const js::mValue& findValue( const std::string& name, const js::mObject& obj )
//{
//    js::mObject::const_iterator i = obj.find(name);

//    assert( i != obj.end() );
//    assert( i->first == name );

//    return i->second;
//}
#endif

std::wstring convertValueToWString( Value& v )
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
