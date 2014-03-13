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

#ifndef MLD_LINKDAO_H
#define MLD_LINKDAO_H

#include <vector>

#include "mld/common.h"
#include "mld/dao/AbstractDao.h"
#include "mld/model/Link.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
}}

namespace mld {

/**
 * @brief The Link dao
 *  Manage CRUD for HLINK and VLINK
 */
class MLD_API LinkDao : public AbstractDao
{
public:
    LinkDao( sparksee::gdb::Graph* g );
    virtual ~LinkDao() override;
    virtual void setGraph( sparksee::gdb::Graph* g ) override;

    HLink addHLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    HLink addHLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt, double weight );
    HLink addHLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt, AttrMap& data );

    VLink addVLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent );
    VLink addVLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, double weight );
    VLink addVLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, AttrMap& data );

    HLink getHLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    HLink getHLink( sparksee::gdb::oid_t hid );
    std::vector<HLink> getHLink( const ObjectsPtr& objs );

    VLink getVLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent );
    VLink getVLink( sparksee::gdb::oid_t vid );
    std::vector<VLink> getVLink( const ObjectsPtr& objs );

    bool removeHLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    bool removeHLink( sparksee::gdb::oid_t hid );

    bool removeVLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    bool removeVLink( sparksee::gdb::oid_t vid );

    bool updateHLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt, double weight );
    bool updateHLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt, AttrMap& data );
    bool updateHLink( sparksee::gdb::oid_t hid, AttrMap& data );
    bool updateHLink( sparksee::gdb::oid_t hid, double weight );

    bool updateVLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, double weight );
    bool updateVLink( sparksee::gdb::oid_t child, sparksee::gdb::oid_t parent, AttrMap& data );
    bool updateVLink( sparksee::gdb::oid_t vid, AttrMap& data );
    bool updateVLink( sparksee::gdb::oid_t vid, double weight );

    inline sparksee::gdb::type_t hlinkType() const { return m_hType; }
    inline sparksee::gdb::type_t vlinkType() const { return m_vType; }

private:
    bool removeLinkImpl( sparksee::gdb::type_t edgeType, sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    bool removeLinkImpl( sparksee::gdb::type_t edgeType, sparksee::gdb::oid_t id );

    sparksee::gdb::oid_t addEdge( sparksee::gdb::type_t lType,
                                  sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );

private:
    sparksee::gdb::type_t m_hType;
    sparksee::gdb::type_t m_vType;

    AttrMap m_hLinkAttr;
    AttrMap m_vLinkAttr;
};

} // end namespace mld

#endif // MLD_LINKDAO_H
