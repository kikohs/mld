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

#include <dex/gdb/Objects.h>

#include "mld/operator/AbstractCoarsener.h"
#include "mld/operator/AbstractSelector.h"
#include "mld/operator/AbstractMerger.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace dex::gdb;

AbstractCoarsener::AbstractCoarsener( dex::gdb::Graph* g )
    : AbstractOperator(g)
    , m_success(false)
    , m_scaleFac(0)
{
}

AbstractCoarsener::~AbstractCoarsener()
{
}

void AbstractCoarsener::pre_exec()
{
    if( !m_sel || !m_merger ) {
        LOG(logERROR) << "AbstractCoarsener::pre_exec: You need to set a \
                         Selector and a Merger in children classes";
        return;
    }
    Layer top = m_dao->mirrorTopLayer();
    if( top.id() != Objects::InvalidOID )
        m_success = true;
}

void AbstractCoarsener::exec()
{
    if( !m_success ) {
        LOG(logERROR) << "AbstractCoarsener::exec: pre_exec phase failed, abort";
        return;
    }
    m_success = false;

    int64_t numVertices = m_dao->getNodeCount(m_dao->baseLayer());
    if( numVertices == kINVALID_NODE_COUNT ) {
        LOG(logERROR) << "AbstractCoarsener::exec: invalid base layer node count";
        return;
    }

    Layer current = m_dao->topLayer();

    // Use default rounding
    uint64_t merge_count = m_scaleFac * numVertices + 1;
    // While there are edge to collapse
    while( merge_count > 0 ) {
        HLink link = m_sel->selectBestHLink(current);
        if( link.id() != Objects::InvalidOID ) {
            bool success = m_merger->merge(link, *m_sel);
            if( !success ) {
                LOG(logERROR) << "AbstractCoarsener::exec: Merger failed to collapse HLink " << link;
                return;
            }
            merge_count--;
        }
        else {
            LOG(logERROR) << "AbstractCoarsener::exec: best HLink is invalid";
            return;
        }
    }
    m_success = true;
}

void AbstractCoarsener::post_exec()
{
    if( !m_success ) {
        LOG(logERROR) << "AbstractCoarsener::post_exec: exec phase failed, abort";
        return;
    }
}
