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

#include "mld/model/GraphObject.h"

namespace mld {
/**
 * @brief The Node concrete class
 */
class MLD_API Node: public GraphObject
{
public:
    Node();
    Node( sparksee::gdb::oid_t id );
    Node( sparksee::gdb::oid_t id, const std::wstring& label, double weight );

    virtual ~Node() override;

    inline bool operator==( const Node& rhs ) const
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

std::ostream& operator<<( std::ostream& out, const mld::Node& sn );

#endif // MLD_SUPERNODE_H
