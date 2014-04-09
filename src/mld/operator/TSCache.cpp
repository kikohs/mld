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

#include <algorithm>
#include <sparksee/gdb/Objects.h>

#include "mld/operator/TSCache.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

const uint64_t MAXUINT = UINT64_MAX;
// here to remove bug in QtCreator's linter

TSCache::TSCache( const std::shared_ptr<MLGDao>& dao )
    : m_dao(dao)
    , m_dir(TSDirection::BOTH)
    , m_radius(0)
    , m_activeLayer(Objects::InvalidOID)
    , m_upperBoundLayer(Objects::InvalidOID)
    , m_entries(0)
    , m_maxEntries(MAXUINT)
{
}

void TSCache::reset( oid_t startLayer, TSDirection dir, size_t radius )
{
    clear();
    m_activeLayer = startLayer;
    m_dir = dir;
    m_radius = radius;
}

void TSCache::clear()
{
    m_cacheMap.clear();
    m_cacheList.clear();
    m_entries = 0;
    m_activeLayer = Objects::InvalidOID;
    m_upperBoundLayer = Objects::InvalidOID;
    m_radius = 0;
    m_dir = TSDirection::BOTH;
}

void TSCache::scrollUp()
{
    auto parent = m_dao->parent(m_activeLayer);

    if( parent == Objects::InvalidOID ) {
        for( auto& p: m_cacheList ) {
            p.second.scroll(); // scroll iterators
            p.second.shrink(); // remove olds values
        }
        return;
    }

    // Update active layer
    m_activeLayer = parent;
    auto bounds = m_dao->getLayerBounds(m_activeLayer, m_dir, m_radius);

    // Already reached top layer
    if( bounds.second == m_upperBoundLayer ) {
        for( auto& p: m_cacheList ) {
            p.second.scroll(); // scroll iterators
            p.second.shrink(); // remove olds values
        }
        return;
    }

    m_upperBoundLayer = bounds.second;
    // Update cache entries
    type_t oType = m_dao->olinkType();
    Value v;

    for( auto& p: m_cacheList ) {
        // Get OLink weight
        oid_t eid = m_dao->findEdge(oType, m_upperBoundLayer, p.first);
        m_dao->graph()->GetAttribute(eid, m_dao->graph()->FindAttribute(oType, Attrs::V[OLinkAttr::WEIGHT]), v);
        // Add new value
        p.second.push_back(v.GetDouble());
        p.second.scroll(); // scroll iterators
        p.second.shrink(); // remove olds values
    }
}

EntryPair TSCache::get( oid_t nid )
{
    auto it = m_cacheMap.find(nid);
    if( it != m_cacheMap.end() ) {  // found in cache
        // it->second is itself an iterator on a cachelist element
        return *it->second;
    }

    auto ts(m_dao->getSignal(nid, m_activeLayer, m_dir, m_radius));
    if( ts.empty() ) {
        LOG(logERROR) << "TSCache::get error nid: " << nid << " lid: " << m_activeLayer;
        return std::make_pair(Objects::InvalidOID, TimeSeries<double>());
    }

    insert(nid, ts);  // insert in cache
    return EntryPair(nid, ts);
}

void TSCache::insert( oid_t nid, const TimeSeries<double>& ts )
{
    // push it to the front;
    m_cacheList.push_front( std::make_pair(nid, ts) );

    // add it to the cache map
    m_cacheMap[nid] = m_cacheList.begin();

    // increase count of entries
    m_entries++;

    // check if it's time to remove the last element
    if( m_entries > m_maxEntries ) {
        // erease from the map the last cache list element
        m_cacheMap.erase( m_cacheList.back().first );

        // erase it from the list
        m_cacheList.pop_back();

        // decrease count
        m_entries--;
    }
}



