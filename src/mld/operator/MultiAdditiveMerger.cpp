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
#include <dex/gdb/ObjectsIterator.h>

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
    std::vector<SuperNode> nodes = m_dao->getNode(neighbors);
    double total = target.weight();
    for( auto& source: nodes ) {
        HLink l = m_dao->getHLink(target.id(), source.id());
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "MultiAdditiveMerger::computeWeight invalid HLink: " << target << " " << source;
        }
#endif
        total += source.weight() * l.weight();
    }
    return total;
}

bool MultiAdditiveMerger::merge( SuperNode& target, const ObjectsPtr& neighbors )
{
#ifdef MLD_FINE_TIMER
    std::unique_ptr<Timer> t(new Timer("MultiAdditiveMerger::merge"));
#endif

#ifdef MLD_SAFE
    if( !neighbors ) {
        LOG(logERROR) << "MultiAdditiveMerger::merge null Object";
    }
#endif
    ObjectsIt it(neighbors->Iterator());
    while( it->HasNext() ) {
        auto srcId = it->Next();
        auto src = m_dao->getNode(srcId);
        // Create new VLINKS to children and parents of source
        if( !m_dao->copyAndMergeVLinks(src, target, false) ) {
            LOG(logERROR) << "MultiAdditiveMerger::merge failed to copy and merge vlinks";
            return false;
        }

        // Create new HLINKS to source's neighbors, add weight for common edges
        if( !m_dao->copyAndMergeHLinks(src, target, false) ) {
            LOG(logERROR) << "MultiAdditiveMerger::merge failed to copy and merge hlinks";
            return false;
        }
    }

    target.setWeight( computeWeight(target, neighbors) );
    // Finally update in db
    m_dao->updateNode(target);

    // Remove contracted source node, it removes all associated relationships
    it.reset(neighbors->Iterator());
    while( it->HasNext() ) {
        m_dao->removeNode(it->Next());
    }
    return true;
}
