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

void HeavyHLinkSelector::setCurrentBestNeighbors()
{
    oid_t best = getBestEnpoint(m_current).second;
    m_curNeighbors->Clear();
    if( best != Objects::InvalidOID )
        m_curNeighbors->Add(best);
}

HeavyHLinkSelector::Endpoint HeavyHLinkSelector::getBestEnpoint( sparksee::gdb::oid_t snid )
{
    // Retrieve best neighbors
    ObjectsPtr nodeSet(getNeighbors(m_current));
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
