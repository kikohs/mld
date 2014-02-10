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

#include "mld/operator/MultiAdditiveMerger.h"
#include "mld/operator/AbstractSelector.h"
#include "mld/model/SuperNode.h"
#include "mld/dao/MLGDao.h"

#ifdef MLD_FINE_TIMER
    #include "mld/utils/Timer.h"
#endif

using namespace mld;
using namespace dex::gdb;

MultiAdditiveMerger::MultiAdditiveMerger( Graph* g )
    : AbstractMultiMerger(g)
{
}

MultiAdditiveMerger::~MultiAdditiveMerger()
{
}

double MultiAdditiveMerger::computeWeight( const SuperNode& target,
                                      const ObjectsPtr& neighbors
                                    )
{
    SuperNode merged(target);
    std::vector<SuperNode> nodes = m_dao->getNode(neighbors);
    double total = merged.weight();
    for( auto& node: nodes ) {
        HLink l = m_dao->getHLink(target.id(), node.id());
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "MultiAdditiveMerger::computeWeight invalid HLink: " << target << " " << node;
        }
#endif
        total += l.weight() * node.weight();
    }
    return total;
}

bool MultiAdditiveMerger::merge( const SuperNode& source, const ObjectsPtr& neighbors )
{
#ifdef MLD_FINE_TIMER
    std::unique_ptr<Timer> t(new Timer("MultiAdditiveMerger::merge"));
#endif
    // TODO
    return false;
}
