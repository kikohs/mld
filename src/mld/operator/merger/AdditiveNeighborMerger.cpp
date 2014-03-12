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

#include <sparksee/gdb/Objects.h>
#include <sparksee/gdb/ObjectsIterator.h>

#include "mld/operator/merger/AdditiveNeighborMerger.h"
#include "mld/model/Node.h"
#include "mld/dao/MLGDao.h"

#ifdef MLD_FINE_TIMER
    #include "mld/utils/Timer.h"
#endif

using namespace mld;
using namespace sparksee::gdb;

AdditiveNeighborMerger::AdditiveNeighborMerger( Graph* g )
    : NeighborMerger(g)
{
}

AdditiveNeighborMerger::~AdditiveNeighborMerger()
{
}

double AdditiveNeighborMerger::computeWeight( const Node& target,
                                      const ObjectsPtr& neighbors
                                    )
{
    std::vector<Node> nodes = m_dao->getNode(neighbors);
    double total = target.weight();
    for( auto& source: nodes ) {
        total += source.weight();
    }
    return total;
}

bool AdditiveNeighborMerger::merge( Node& target, const ObjectsPtr& neighbors )
{
#ifdef MLD_FINE_TIMER
    std::unique_ptr<Timer> t(new Timer("AdditiveNeighborMerger::merge"));
#endif

#ifdef MLD_SAFE
    if( !neighbors ) {
        LOG(logERROR) << "AdditiveNeighborMerger::merge null Object";
        return false;
    }
#endif
    double total = target.weight();
    ObjectsIt it(neighbors->Iterator());
    while( it->HasNext() ) {
        auto id = it->Next();
        Node src = m_dao->getNode(id);
        if( src.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "AdditiveNeighborMerger::merge invalid node: " << id;
            return false;
        }
        // Create new VLINKS to children and parents of source
        if( !m_dao->horizontalCopyVLinks(src, target, false) ) {
            LOG(logERROR) << "AdditiveNeighborMerger::merge failed to copy and merge vlinks";
            return false;
        }

        // Create new HLINKS to source's neighbors, add weight for common edges
        if( !m_dao->horizontalCopyHLinks(src, target, false) ) {
            LOG(logERROR) << "AdditiveNeighborMerger::merge failed to copy and merge hlinks";
            return false;
        }

        total += src.weight();
        // Remove contracted source node, it removes all associated relationships
        m_dao->removeNode(src.id());
    }

    target.setWeight(total);
    // Finally update in db
    m_dao->updateNode(target);
    return true;
}
