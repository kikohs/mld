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

#ifndef MLD_NODEDAO_H
#define MLD_NODEDAO_H

#include <vector>

#include "mld/common.h"
#include "mld/dao/AbstractDao.h"
#include "mld/model/Node.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
}}

namespace mld {

class MLD_API NodeDao : public AbstractDao
{
public:
    NodeDao( sparksee::gdb::Graph* g );
    ~NodeDao();
    virtual void setGraph( sparksee::gdb::Graph* g ) override;
    Node addNode();
    void removeNode( sparksee::gdb::oid_t id );
    void updateNode( Node& n );
    Node getNode( sparksee::gdb::oid_t id );
    std::vector<Node> getNode( const ObjectsPtr& objs );
    sparksee::gdb::type_t superNodeType() const { return m_nType; }

private:
    sparksee::gdb::type_t m_nType;
};

} // end namespace mld

#endif // MLD_NODEDAO_H
