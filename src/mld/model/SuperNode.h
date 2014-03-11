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

#ifndef MLD_SUPERNODE_H
#define MLD_SUPERNODE_H

#include <sparksee/gdb/common.h>
#include "mld/common.h"

namespace mld {

/**
 * @brief Node abstract class
 */
class MLD_API Node
{
public:
    virtual ~Node() = 0;

    void setId( sparksee::gdb::oid_t id ) { m_id = id; }
    inline sparksee::gdb::oid_t id() const { return m_id; }

protected:
    Node();
    Node( sparksee::gdb::oid_t id );

protected:
    sparksee::gdb::oid_t m_id;
};

/**
 * @brief The SuperNode concrete class
 */
class MLD_API SuperNode: public Node
{
public:
    SuperNode();
    SuperNode( sparksee::gdb::oid_t id );
    SuperNode( sparksee::gdb::oid_t id, const std::wstring& label, double weight );

    virtual ~SuperNode() override;

    inline bool operator==( const SuperNode& rhs ) const
    {
        return ( (m_id == rhs.id())
                 && (m_weight == rhs.m_weight)
                 && (m_label == rhs.m_label)
                 ? true : false );
    }

    inline double weight() const { return m_weight; }
    void setWeight( double v ) { m_weight = v; }
    inline std::wstring label() const { return m_label; }
    void setLabel( const std::wstring& label ) { m_label = label; }

private:
    double m_weight;
    std::wstring m_label;
};

} // end namespace mld

std::ostream& operator<<( std::ostream& out, const mld::SuperNode& sn );

#endif // MLD_SUPERNODE_H
