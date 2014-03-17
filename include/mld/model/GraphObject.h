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

#ifndef MLD_GRAPHOBJECT_H
#define MLD_GRAPHOBJECT_H

#include <map>
#include <sparksee/gdb/Value.h>

#include "mld/common.h"
#include "mld/GraphTypes.h"

namespace mld {

typedef std::map<std::wstring, sparksee::gdb::Value> AttrMap;

class MLD_API GraphObject
{
public:
    virtual ~GraphObject() = 0;
    inline sparksee::gdb::oid_t id() const { return m_id; }
    inline const AttrMap& data() const { return m_data; }
    inline AttrMap& data() { return m_data; }

    void setId( sparksee::gdb::oid_t id ) { m_id = id; }
    void setData( const AttrMap& data ) { m_data = data; }

    /**
     * @brief BEWARE, does not compare text in Value object
     * @param rhs
     * @return equality
     */
    inline bool operator==( const GraphObject& rhs ) const
    {
        auto pred = []( decltype(*m_data.begin()) a, decltype(a) b ) {
                        return a.first == b.first && a.second.Equals(b.second);
        };
        return m_id == rhs.m_id
                && m_data.size() == rhs.m_data.size()
                && std::equal(m_data.begin(), m_data.end(), rhs.m_data.begin(), pred);
    }

    inline bool operator !=( const GraphObject& rhs ) const
    {
        return !operator ==(rhs);
    }
    /**
     * @brief Print method which display the content of AttrMap and the id
     * @param out
     */
    virtual void print( std::ostream& out ) const;

protected:
    GraphObject();
    GraphObject( sparksee::gdb::oid_t id );
    GraphObject( sparksee::gdb::oid_t id, const AttrMap& data );

protected:
    sparksee::gdb::oid_t m_id;
    mutable AttrMap m_data;
};

} // end namespace mld

std::ostream& operator<<( std::ostream& out, const mld::GraphObject& sn );

#endif // MLD_GRAPHOBJECT_H
