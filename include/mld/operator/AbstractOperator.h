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

#ifndef MLD_ABSTRACTOPERATOR_H
#define MLD_ABSTRACTOPERATOR_H

#include "mld/common.h"

namespace dex {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
}}

namespace mld {

class MLGDao;

/**
 * @brief Abstract MLG Operator
 *  Base class for Coarseners or Expanders and Refiners
 */
class MLD_API AbstractOperator
{
public:
    AbstractOperator( dex::gdb::Graph* g );
    virtual ~AbstractOperator() = 0;

    // Disable copy and assignement ctor
    AbstractOperator( const AbstractOperator& ) = delete;
    AbstractOperator& operator=( const AbstractOperator& ) = delete;

    /**
     * @brief Run the underlying algorithms
     *  Call pre_exec(), exec() and post_exec()
     */
    void run();

    /**
     * @brief Pre execution process
     *  To reimplement
     */
    virtual void pre_exec() = 0;

    /**
     * @brief Main method to be reimplemented
     */
    virtual void exec() = 0;

    /**
     * @brief Run after exec, post execution method
     */
    virtual void post_exec() = 0;

    /**
     * @brief Scale Factor, for coarsener or expanders
     *  The factor is always calculated from the Base layer
     * E.g 0.9 means that the goal is to keep 90% of base layer nodes
     * @param fac
     */
    void setScaleFactor( float fac ) { m_scaleFac = fac; }
    float scaleFactor() const { return m_scaleFac; }

private:
    dex::gdb::Graph* m_g;
    std::unique_ptr<MLGDao> m_dao;
    float m_scaleFac;
};

} // end namespace mld

#endif // MLD_ABSTRACTOPERATOR_H
