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

#include <sparksee/gdb/Graph.h>
#include <sparksee/gdb/Graph_data.h>
#include <sparksee/gdb/Objects.h>
#include <sparksee/gdb/ObjectsIterator.h>

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

    template<typename T>
    T addLink( sparksee::gdb::type_t lType,
               sparksee::gdb::oid_t src,
               sparksee::gdb::oid_t tgt, AttrMap& data, bool updateDb )
    {
        sparksee::gdb::oid_t eid = addEdge(lType, src, tgt);
        if( eid == sparksee::gdb::Objects::InvalidOID )
            return T();
        if( updateDb ) {
            if( !updateAttrMap(lType, eid, data) )
                return T();
        }
        return T(eid, src, tgt, data);
    }

    template<typename T>
    T getLink( sparksee::gdb::type_t lType,
               sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt )
    {
        sparksee::gdb::oid_t eid = findEdge(lType, src, tgt);
        if( eid == sparksee::gdb::Objects::InvalidOID )
            return T();
        return T(eid, src, tgt, readAttrMap(eid));
    }

    template<typename T>
    T getLink( sparksee::gdb::type_t lType, sparksee::gdb::oid_t eid )
    {
        #ifdef MLD_SAFE
            if( eid == sparksee::gdb::Objects::InvalidOID ) {
                LOG(logERROR) << "AbstractDao::getLink: invalid oid";
                return T();
            }
        #endif
            std::unique_ptr<sparksee::gdb::EdgeData> eData;
        #ifdef MLD_SAFE
            try {
                if( m_g->GetObjectType(eid) != lType ) {
                    LOG(logERROR) << "AbstractDao::getLink: invalid type: " << lType << " "
                                  << "object type: " << m_g->GetObjectType(eid);
                    return T();
                }
        #endif
                eData.reset(m_g->GetEdgeData(eid));
        #ifdef MLD_SAFE
            } catch( sparksee::gdb::Error& e ) {
                LOG(logERROR) << "AbstractDao::getLink: " << e.Message();
                return T();
            }
        #endif
            return T(eid, eData->GetTail(), eData->GetHead(), readAttrMap(eid));
    }

    template<typename T>
    std::vector<T> getLink( sparksee::gdb::type_t lType, const ObjectsPtr& objs )
    {
        std::vector<T> res;
        res.reserve(objs->Count());
        ObjectsIt it(objs->Iterator());
        while( it->HasNext() ) {
            T e(getLink<T>(lType, it->Next()));
        #ifdef MLD_SAFE
            // Watch for the if, no { }
            if( e.id() != sparksee::gdb::Objects::InvalidOID )
        #endif
                res.push_back(e);
        }
        return res;
    }

protected:
    sparksee::gdb::Graph* m_g;
    std::unique_ptr<sparksee::gdb::Value> m_v;
};

} // end namespace mld

#endif // MLD_ABSTRACTDAO_H
