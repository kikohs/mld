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
#include <dex/gdb/ObjectsIterator.h>
#include <dex/gdb/Graph_data.h>
#include <dex/gdb/Value.h>

#include "mld/operator/mergers.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace dex::gdb;

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

void createVLinkFromSrc( const SuperNode& target, const SuperNode& source, MLGDao* dao )
{
    // Add vlink to source's children
    for( auto& c: dao->getChildNodes(source.id()) ) {
        dao->addVLink(c, target);
    }
    // Add VLinks to source's parents
    for( auto& p: dao->getParentNodes(source.id()) ) {
        dao->addVLink(target, p);
    }
}

void createHLinkFromSrc( const SuperNode& target, const SuperNode& source, MLGDao* dao )
{
    // Get target's neighbors
    ObjectsPtr tgtNeighbors(dao->graph()->Neighbors(target.id(), dao->hlinkType(), Outgoing));
    // Get source's neighbors
    ObjectsPtr srcNeighbors(dao->graph()->Neighbors(source.id(), dao->hlinkType(), Outgoing));

    // Get common neighbors
    ObjectsPtr commonNeighbors( Objects::CombineIntersection(tgtNeighbors.get(), srcNeighbors.get()) );
    // If any merge edge weight on target node
    if( commonNeighbors->Count() != 0 ) {
        ObjectsIt it(commonNeighbors->Iterator());
        while( it->HasNext() ) {
            oid_t common = it->Next();
            HLink tLink = dao->getHLink(target.id(), common);
            HLink sLink = dao->getHLink(source.id(), common);
            // Add weight and update HLink
            bool ok = dao->updateHLink(tLink.id(), tLink.weight() + sLink.weight() );
            if( !ok ) {
                LOG(logERROR) << "BasicAdditiveMerger:createHLinkFromSrc update HLink failed: " << tLink;
            }
        }
    }

    // Add the rest of source's HLinks (remove common neighbors)
    // Rremove target id
    srcNeighbors->Remove(target.id());
    ObjectsPtr onlySrcNeighbors( Objects::CombineDifference(srcNeighbors.get(), commonNeighbors.get()) );
    if( onlySrcNeighbors->Count() != 0 ) {
        ObjectsIt it(onlySrcNeighbors->Iterator());
        // No self-loops allowed
        while( it->HasNext() ) {
            oid_t n = it->Next();
            HLink sLink = dao->getHLink(source.id(), n);
            // Create new hlink
            dao->addHLink(target, SuperNode(n), sLink.weight());
        }
    }
}

} // end namespace anonymous

BasicAdditiveMerger::BasicAdditiveMerger( dex::gdb::Graph* g )
    : AbstractSingleMerger(g)
{
}

BasicAdditiveMerger::~BasicAdditiveMerger()
{
}

bool BasicAdditiveMerger::merge( const HLink& hlink, const AbstractSingleSelector& selector )
{
    if( hlink.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "BasicAdditiveMerger::merge: invalid HLink";
        return false;
    }

    SuperNode src;
    SuperNode tgt;
    std::tie(tgt, src) = getHeaviestNodeFirst(hlink, m_dao.get());

    // Create new VLINKS to children and parents of source
    createVLinkFromSrc(tgt, src, m_dao.get());

    // Create new HLINKS to source's neighbors, add weight for common edges
    createHLinkFromSrc(tgt, src, m_dao.get());

    // Update target weight
    tgt.setWeight(tgt.weight() + hlink.weight());
    m_dao->updateNode(tgt);

    // Remove contracted source node, it removes all associated relationships
    m_dao->removeNode(src.id());

    // TODO
    // Update selector cache
    // update tgt node
    // remove src node and associated hlinks
    MLD_UNUSED(selector);
    return true;
}
