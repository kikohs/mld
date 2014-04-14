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
//    friend class MLGDao;
//    friend class NodeDao;
public:
    explicit Node();
    /**
     * @brief Node ctor called by the Dao
     * @param id
     * @param data
     */
    explicit Node( sparksee::gdb::oid_t id, const AttrMap& data );
    // "Hide" setId method
    using GraphObject::setId;
    virtual ~Node() override;

    inline double weight() const { return m_data[Attrs::V[NodeAttr::WEIGHT]].GetDouble(); }
    void setWeight( double v );
    inline std::wstring label() const { return m_data[Attrs::V[NodeAttr::LABEL]].GetString(); }
    void setLabel( const std::wstring& label );
};

} // end namespace mld

#endif // MLD_SUPERNODE_H
