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
    typedef sparksee::gdb::oid_t oid_t;
    typedef sparksee::gdb::type_t type_t;
    typedef sparksee::gdb::Objects Objects;

public:
    LinkDao( sparksee::gdb::Graph* g );
    virtual ~LinkDao() override;
    virtual void setGraph( sparksee::gdb::Graph* g ) override;

    // ***** HLINK ***** //
    inline type_t hlinkType() const { return m_hType; }
    HLink addHLink( oid_t src, oid_t tgt );
    HLink addHLink( oid_t src, oid_t tgt, double weight );
    HLink addHLink( oid_t src, oid_t tgt, AttrMap& data );

    HLink getHLink( oid_t src, oid_t tgt );
    HLink getHLink( oid_t hid );
    std::vector<HLink> getHLink( const ObjectsPtr& objs );

    bool updateHLink( oid_t src, oid_t tgt, double weight );
    bool updateHLink( oid_t src, oid_t tgt, AttrMap& data );
    bool updateHLink( oid_t hid, AttrMap& data );
    bool updateHLink( oid_t hid, double weight );

    bool removeHLink( oid_t src, oid_t tgt );
    bool removeHLink( oid_t hid );

    // ***** VLINK ***** //
    inline type_t vlinkType() const { return m_vType; }
    VLink addVLink( oid_t child, oid_t parent );
    VLink addVLink( oid_t child, oid_t parent, double weight );
    VLink addVLink( oid_t child, oid_t parent, AttrMap& data );

    VLink getVLink( oid_t child, oid_t parent );
    VLink getVLink( oid_t vid );
    std::vector<VLink> getVLink( const ObjectsPtr& objs );

    bool updateVLink( oid_t child, oid_t parent, double weight );
    bool updateVLink( oid_t child, oid_t parent, AttrMap& data );
    bool updateVLink( oid_t vid, AttrMap& data );
    bool updateVLink( oid_t vid, double weight );

    bool removeVLink( oid_t src, oid_t tgt );
    bool removeVLink( oid_t vid );

    // ***** OLINK ***** //
    inline type_t olinkType() const { return m_oType; }
    OLink addOLink( oid_t layer, oid_t node );
    OLink addOLink( oid_t layer, oid_t node, double weight );
    OLink addOLink( oid_t layer, oid_t node, AttrMap& data );

    OLink getOLink( oid_t layer, oid_t node );
    OLink getOLink( oid_t eid );
    std::vector<OLink> getOLink( const ObjectsPtr& objs );

    bool updateOLink( oid_t layer, oid_t node, double weight );
    bool updateOLink( oid_t layer, oid_t node, AttrMap& data );
    bool updateOLink( oid_t eid, AttrMap& data );
    bool updateOLink( oid_t eid, double weight );

    bool removeOLink( oid_t src, oid_t tgt );
    bool removeOLink( oid_t eid );

private:


private:
    type_t m_hType; // hlinkType
    type_t m_vType; // vlinkType
    type_t m_oType; // ownsType

    AttrMap m_hLinkAttr;
    AttrMap m_vLinkAttr;
    AttrMap m_oLinkAttr;
};

} // end namespace mld

#endif // MLD_LINKDAO_H
