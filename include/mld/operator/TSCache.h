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

#ifndef MLD_TSCACHE_H
#define MLD_TSCACHE_H

#include <list>
#include <unordered_map>

#include <sparksee/gdb/Graph_data.h>

#include "mld/common.h"
#include "mld/model/TimeSeries.h"

namespace mld {

using EntryPair = std::pair<sparksee::gdb::oid_t, TimeSeries<double>>;
using CacheList = std::list<EntryPair>;
using CacheMap = std::unordered_map<sparksee::gdb::oid_t, CacheList::iterator>;

class MLD_API TSCache
{
public:
    TSCache();

private:
    uint64_t m_entries;
    uint64_t m_maxEntries;
    /// Cache LRU list
    CacheList m_cacheList;
    /// Cache map into the list
    CacheMap m_cacheMap;
};

} // end namespace mld

#endif // MLD_TSCACHE_H
