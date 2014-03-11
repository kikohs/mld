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

#include "mld/operator/selector/HeavyHLinkSelector.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

HeavyHLinkSelector::HeavyHLinkSelector( Graph* g )
    : NeighborSelector(g)
{
}

HeavyHLinkSelector::~HeavyHLinkSelector()
{
}

double HeavyHLinkSelector::calcScore( oid_t snid )
{
    return getBestEnpoint(snid).first;
}

HeavyHLinkSelector::Endpoint HeavyHLinkSelector::getBestEnpoint( sparksee::gdb::oid_t snid )
{
    if( snid == Objects::InvalidOID ) {
        LOG(logERROR) << "HeavyHLinkSelector::getBestEnpoint invalid oid";
        return Endpoint(0.0, Objects::InvalidOID);
    }
    // Retrieve best neighbors
    ObjectsPtr nodeSet(getNeighbors(snid));
    ObjectsIt it(nodeSet->Iterator());
    oid_t best = Objects::InvalidOID;
    double weight = 0.0;
    while( it->HasNext() ) {
        auto id = it->Next();
        HLink link = m_dao->getHLink(snid, id);
        double tmp = link.weight();
        if( tmp >= weight ) {
            weight = tmp;
            best = id;
        }
    }
    return Endpoint(weight, best);
}

void HeavyHLinkSelector::setNodesToMerge()
{
    oid_t best = getBestEnpoint(m_root).second;
    m_curNeighbors->Clear();
    if( best != Objects::InvalidOID )
        m_curNeighbors->Add(best);
}

void HeavyHLinkSelector::setNodesToUpdate()
{
    // Should update only common neighbors between
    // root and other endpoint
    // Looking for triangles
    if( m_curNeighbors->Count() == 0 )
        return;

    oid_t best = m_curNeighbors->Any();  // Only one node in curNeighbors
    m_nodesToUpdate = getNeighbors(m_root);
    auto bestN = getNeighbors(best);
    // Keep only common neighbors
    m_nodesToUpdate->Intersection(bestN.get());
}

bool HeavyHLinkSelector::updateScores()
{
    if( m_root == Objects::InvalidOID ) {
        LOG(logERROR) << "HeavyHLinkSelector::updateScores invalid root";
        return false;
    }

    // Update root node, the merging already occured
    if( !m_hasMemory )
        m_scores.update(m_root, calcScore(m_root));

    ObjectsIt it(m_nodesToUpdate->Iterator());
    while( it->HasNext() ) {
        auto id = it->Next();

        if( id == m_root ) // no self loop
            continue;

        // Retrieve HLink between root node and current id
        HLink link = m_dao->getHLink(m_root, id);
#ifdef MLD_SAFE
        if( link.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "HeavyHLinkSelector::updateScores invalid hlink";
            return false;
        }
#endif
        // Retrieve actual score for node to update
        // Update only if HLink weight from root node
        // to current > previous score
        auto it2 = m_scores.findByVal(id);
#ifdef MLD_SAFE
        if( it2 == m_scores.valEndIterator() ) {
            LOG(logERROR) << "HeavyHLinkSelector::updateScores "
                          << id << " not found in score queue";
            return false;
        }
#endif
        if( link.weight() > it2->second )
            m_scores.update(id, link.weight());
    }
    return true;
}
