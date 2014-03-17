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

#ifndef MLD_ABSTRACTDAO_H
#define MLD_ABSTRACTDAO_H

#include "mld/common.h"
#include "mld/model/GraphObject.h"

namespace mld {
/**
 * @brief Abstract dao class, implemented by all daos
 */
class MLD_API AbstractDao
{
public:
    virtual ~AbstractDao() = 0;
    AbstractDao( const AbstractDao& ) = delete;
    AbstractDao& operator=( const AbstractDao& ) = delete;

    virtual void setGraph( sparksee::gdb::Graph* g );
    /**
     * @brief Give access to the inner sparksee graph handle
     * @return Graph
     */
    sparksee::gdb::Graph* graph() const { return m_g; }

    /**
     * @brief Read the attribute map of a GraphObject
     * @param id
     * @return Attribute map
     */
    AttrMap readAttrMap( sparksee::gdb::oid_t id );
    /**
     * @brief Persist the attribute map of a graph Object
     * only the valid attributes are commited in the database
     * @param objType Object Type (node or edge)
     * @param id GraphObject id
     * @param data data to update
     * @return success
     */
    bool updateAttrMap( sparksee::gdb::type_t objType, sparksee::gdb::oid_t id, AttrMap& data );


    /**
     * @brief Find Edge, safe version if MLD_SAFE is enabled
     * @param objType
     * @param src
     * @param tgt
     * @return edge oid
     */
    sparksee::gdb::oid_t findEdge( sparksee::gdb::type_t objType,
                                   sparksee::gdb::oid_t src,
                                   sparksee::gdb::oid_t tgt );

protected:
    AbstractDao( sparksee::gdb::Graph* g );
    sparksee::gdb::oid_t addEdge( sparksee::gdb::type_t lType,
                                  sparksee::gdb::oid_t src,
                                  sparksee::gdb::oid_t tgt );

    bool removeEdge( sparksee::gdb::type_t lType,
                     sparksee::gdb::oid_t src,
                     sparksee::gdb::oid_t tgt );

    bool removeEdge( sparksee::gdb::type_t lType,
                     sparksee::gdb::oid_t id );

protected:
    sparksee::gdb::Graph* m_g;
    std::unique_ptr<sparksee::gdb::Value> m_v;
};

} // end namespace mld

#endif // MLD_ABSTRACTDAO_H
