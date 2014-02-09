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

#include <iostream>
#include <dex/gdb/Graph.h>

#include "mld/common.h"

namespace mld {

class MLD_API GraphImporter
{
public:
    GraphImporter();
    /**
     * @brief Import a SNAP graph into MLD
     * @param g Graph handle
     * @param filepath Graph to import
     * @param logpath Import log file
     * @return success
     */
    static bool fromSnapFormat(dex::gdb::Graph* g, const std::string& filepath );
};

} // end namespace mld
#endif // MLD_GRAPHIMPORTER_H
