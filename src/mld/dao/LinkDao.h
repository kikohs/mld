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

namespace dex {
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
    LinkDao( dex::gdb::Graph* g );
    virtual ~LinkDao() override;

    HLink addHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt );
    HLink addHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight );

    VLink addVLink( dex::gdb::oid_t child, dex::gdb::oid_t parent );
    VLink addVLink( dex::gdb::oid_t child, dex::gdb::oid_t parent, double weight );

    HLink getHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt );
    HLink getHLink( dex::gdb::oid_t hid );
    std::vector<HLink> getHLink( const ObjectsPtr& objs );
    HLink getOrCreateHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight );

    VLink getVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt );
    VLink getVLink( dex::gdb::oid_t vid );
    std::vector<VLink> getVLink( const ObjectsPtr& objs );

    bool removeHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt );
    bool removeHLink( dex::gdb::oid_t hid );

    bool removeVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt );
    bool removeVLink( dex::gdb::oid_t vid );

    bool updateHLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight );
    bool updateHLink( dex::gdb::oid_t hid, double weight );

    bool updateVLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight );
    bool updateVLink( dex::gdb::oid_t vid, double weight );

    dex::gdb::type_t hlinkType() const { return m_hType; }
    dex::gdb::type_t vlinkType() const { return m_vType; }

private:
    double getWeight( dex::gdb::type_t edgeType, dex::gdb::oid_t id );
    bool setWeight( dex::gdb::type_t edgeType, dex::gdb::oid_t id, double weight );

    bool removeLinkImpl( dex::gdb::type_t edgeType, dex::gdb::oid_t src, dex::gdb::oid_t tgt );
    bool removeLinkImpl( dex::gdb::type_t edgeType, dex::gdb::oid_t id );

private:
    dex::gdb::type_t m_hType;
    dex::gdb::type_t m_vType;
};

} // end namespace mld

#endif // MLD_LINKDAO_H
