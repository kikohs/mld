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

#ifndef MLD_GRAPHEXPORTER_H
#define MLD_GRAPHEXPORTER_H

#include <map>
#include <sparksee/gdb/Graph.h>

#include "mld/common.h"

namespace mld {

class MLD_API GraphExporter
{
public:
    using RIndexMap = std::map<sparksee::gdb::oid_t, std::wstring>;
    /**
     * @brief Export the multigraph to the timeseries format (2 CSV files)
     * @param g
     * @param name Name of the dataset
     * @param nodePath
     * @param edgePath
     * @return
     */
    static bool toTimeSeries( sparksee::gdb::Graph* g,
                              const std::string& name, std::string& exportFolderPath );
private:
    static bool writeTSNodes( sparksee::gdb::Graph* g,
                              const std::string& nodePath, RIndexMap& indexMap );
    static bool writeTSEdges( sparksee::gdb::Graph* g,
                              const std::string& edgePath, RIndexMap& indexMap );
};

} // end namespace mld

#endif // MLD_GRAPHEXPORTER_H
