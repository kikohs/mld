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

#include <sparksee/gdb/Graph.h>
#include <sparksee/gdb/Objects.h>
#include <sparksee/gdb/ObjectsIterator.h>
#include <sparksee/gdb/Graph_data.h>
#include <sparksee/gdb/Value.h>

#include "mld/operator/mergers.h"
#include "mld/dao/MLGDao.h"

#ifdef MLD_FINE_TIMER
    #include "mld/utils/Timer.h"
#endif

using namespace mld;
using namespace sparksee::gdb;

namespace {

std::pair<SuperNode, SuperNode> getHeaviestNodeFirst( const HLink& hlink, MLGDao* dao )
{
    // Get associated supernodes
    SuperNode n1 = dao->getNode(hlink.source());
    SuperNode n2 = dao->getNode(hlink.target());

    // Heaviest SuperNode attracts the other
    if( n1.weight() > n2.weight() ) {
        return std::make_pair(n1, n2);
    }
    else {
        return std::make_pair(n2, n1);
    }
}

} // end namespace anonymous

BasicAdditiveMerger::BasicAdditiveMerger( sparksee::gdb::Graph* g )
    : AbstractSingleMerger(g)
{
}

BasicAdditiveMerger::~BasicAdditiveMerger()
{
}

double BasicAdditiveMerger::computeWeight( const SuperNode& target, const SuperNode& source )
{
    return target.weight() + source.weight();
}

bool BasicAdditiveMerger::merge( const HLink& hlink, const AbstractSingleSelector& selector )
{
#ifdef MLD_FINE_TIMER
    std::unique_ptr<Timer> t(new Timer("BasicAdditiveMerger::merge"));
#endif
    if( hlink.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "BasicAdditiveMerger::merge: invalid HLink";
        return false;
    }

    SuperNode src;
    SuperNode tgt;
    std::tie(tgt, src) = getHeaviestNodeFirst(hlink, m_dao.get());

    // Create new VLINKS to children and parents of source
    m_dao->horizontalCopyVLinks(src, tgt, false);

    // Create new HLINKS to source's neighbors, add weight for common edges
    m_dao->horizontalCopyHLinks(src, tgt, false);

    // Update target weight
    tgt.setWeight( computeWeight(tgt, src) );
    m_dao->updateNode(tgt);

    // Remove contracted source node, it removes all associated relationships
    m_dao->removeNode(src.id());

    MLD_UNUSED(selector);
    return true;
}
