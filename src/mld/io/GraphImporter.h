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

#ifndef MLD_GRAPHIMPORTER_H
#define MLD_GRAPHIMPORTER_H

#include <map>
#include <sparksee/gdb/Graph.h>

#include "mld/common.h"

namespace mld {

class MLD_API GraphImporter
{
public:
    typedef std::map<uint64_t, sparksee::gdb::oid_t> IndexMap;
    /**
     * @brief Import a SNAP graph
     * @param g Graph handle
     * @param filepath Graph to import
     * @return success
     */
    static bool fromSnapFormat( sparksee::gdb::Graph* g, const std::string& filepath );

    /**
     * @brief Import a timeSeries graph from 2 files
     * name.nodes.csv and name.edges.csv
     * @param g Graph handle
     * @param nodePath filepath to the *.nodes.csv file
     * @param edgePath filepath to the *.edges.csv file
     * @return success
     */
    static bool fromTimeSeries( sparksee::gdb::Graph* g, const std::string& nodePath,
                                const std::string& edgePath, bool autoCreateAttributes=true );

private:
    static bool importTSNodes( sparksee::gdb::Graph* g,
                               const std::string& nodePath, IndexMap& indexMap, bool autoCreateAttributes );
    static bool importTSEdges( sparksee::gdb::Graph* g,
                               const std::string& edgePath, const IndexMap& indexMap, bool autoCreateAttributes );
};

} // end namespace mld
#endif // MLD_GRAPHIMPORTER_H
