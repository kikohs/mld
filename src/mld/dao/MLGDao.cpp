/****************************************************************************
**
** Copyright (C) 2013 EPFL-LTS2
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

#include <dex/gdb/Graph.h>
#include <dex/gdb/Objects.h>
#include <dex/gdb/Graph_data.h>
#include <dex/gdb/Value.h>

#include "mld/Graph_types.h"

#include "mld/dao/MLGDao.h"
#include "mld/dao/LayerDao.h"
#include "mld/dao/SNodeDao.h"

using namespace mld;

MLGDao::MLGDao( dex::gdb::Graph* g )
    : AbstractDao(g)
    , m_sndao( new SNodeDao(g) )
    , m_ldao( new LayerDao(g) )
{
}

MLGDao::~MLGDao()
{
    // DO NOT DELETE GRAPH
}
