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

#ifndef MLD_OWNSDAO_H
#define MLD_OWNSDAO_H

#include "mld/common.h"
#include "mld/dao/AbstractDao.h"
#include "mld/model/Link.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
}}

namespace mld {

class MLD_API OwnsDao : public AbstractDao
{
public:
    OwnsDao( sparksee::gdb::Graph* g );
    virtual ~OwnsDao() override;
    virtual void setGraph( sparksee::gdb::Graph* g ) override;

    OLink addOLink( sparksee::gdb::oid_t layerId, sparksee::gdb::oid_t nodeId );
    OLink addOLink( sparksee::gdb::oid_t layerId, sparksee::gdb::oid_t nodeId, AttrMap& data );
    OLink getOLink( sparksee::gdb::oid_t layerId, sparksee::gdb::oid_t nodeId );
    OLink getOLink( sparksee::gdb::oid_t ownsId );
    void removeOLink( sparksee::gdb::oid_t layerId, sparksee::gdb::oid_t nodeId );

    inline sparksee::gdb::type_t ownsType() const { return m_ownsType; }

private:
    sparksee::gdb::type_t m_ownsType;
    AttrMap m_ownsAttr;
};

} // end namespace mld

#endif // MLD_OWNSDAO_H
