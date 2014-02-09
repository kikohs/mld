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

#include <iostream>
#include <fstream>

#include "mld/io/GraphImporter.h"
#include "mld/Graph_types.h"
#include "mld/utils/Timer.h"
#include "mld/dao/MLGDao.h"
#include "mld/dao/LinkDao.h"

using namespace mld;
using namespace dex::gdb;

namespace {

oid_t addOrGetFromIndexMap( uint64_t node, std::map<uint64_t, oid_t>& indexMap,
                            const Layer& base, MLGDao& dao )
{
    oid_t id = 0;
    auto it = indexMap.find(node);
    if( it != indexMap.end() ) {
        id = it->second;
    }
    else {  // Not in map
        // Add to graph
        SuperNode n = dao.addNodeToLayer(base);
        std::wstringstream stream;
        stream << node;
        n.setLabel(stream.str());
        dao.updateNode(n);
        indexMap[node] = n.id();
        id = n.id();
    }
    return id;
}

} // end namespace anonymous

GraphImporter::GraphImporter()
{
}

bool GraphImporter::fromSnapFormat( dex::gdb::Graph* g, const std::string& filepath )
{
    std::unique_ptr<Timer> t(new Timer("Importing graph"));
    LOG(logINFO) << "Parsing SNAP undirected graph: " << filepath;
    MLGDao dao(g);
    LinkDao linkDao(g);
    dao.addBaseLayer();
    Layer base = dao.baseLayer();

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

    std::map<uint64_t, oid_t> indexMap;
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
