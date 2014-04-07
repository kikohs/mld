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
#include "mld/operator/TSCache.h"

using namespace mld;
using namespace sparksee::gdb;

TSCache::TSCache( const std::shared_ptr<MLGDao>& dao )
    : m_dao(dao)
    , m_entries(0)
    , m_maxEntries(UINT64_MAX)
{
}

void TSCache::insert( oid_t nid, const TimeSeries<double>& ts )
{
    // WriteLock w_lock(m_lock);
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

EntryPair TSCache::get( oid_t nid )
{
    // ReadLock r_lock(m_lock);
    auto it = m_cacheMap.find(nid);
    if( it != m_cacheMap.end() ) {
        // it->second is itself an iterator on a cachelist element
        return *it->second;
    }
    else {
        return std::make_pair(Objects::InvalidOID, TimeSeries<double>());
    }
}
