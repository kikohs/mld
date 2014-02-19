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

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
}}

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

protected:
    AbstractDao( sparksee::gdb::Graph* g );

protected:
    sparksee::gdb::Graph* m_g;
    std::unique_ptr<sparksee::gdb::Value> m_v;
};

} // end namespace mld

#endif // MLD_ABSTRACTDAO_H
