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

#ifndef MLD_SNODEDAO_H
#define MLD_SNODEDAO_H

#include <vector>

#include "mld/common.h"
#include "mld/dao/AbstractDao.h"
#include "mld/model/SuperNode.h"

namespace dex {
namespace gdb {
    class Graph;
    class Value;
}}

namespace mld {

class MLD_API SNodeDao : public AbstractDao
{
public:
    SNodeDao( dex::gdb::Graph* g );
    ~SNodeDao();

    SuperNode addNode();
    void removeNode( dex::gdb::oid_t id );
    void updateNode( const SuperNode& n );
    SuperNode getNode( dex::gdb::oid_t id );
    std::vector<SuperNode> getNode( const ObjectsPtr& objs );

    dex::gdb::type_t superNodeType() const { return m_snType; }

private:
    dex::gdb::type_t m_snType;
};

} // end namespace mld

#endif // MLD_SNODEDAO_H
