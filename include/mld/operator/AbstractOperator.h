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

namespace sparksee {
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
    AbstractOperator( sparksee::gdb::Graph* g );
    virtual ~AbstractOperator() = 0;

    // Disable copy and assignement ctor
    AbstractOperator( const AbstractOperator& ) = delete;
    AbstractOperator& operator=( const AbstractOperator& ) = delete;

    /**
     * @brief Run the underlying algorithms
     *  Call pre_exec(), exec() and post_exec()
     * @return success
     */
    bool run();

protected:
    /**
     * @brief Pre execution process
     *  To reimplement
     *  @return success
     */
    virtual bool preExec() = 0;

    /**
     * @brief Main method to be reimplemented
     * @return success
     */
    virtual bool exec() = 0;

    /**
     * @brief Run after exec, post execution method
     * @return success
     */
    virtual bool postExec() = 0;

protected:
    std::unique_ptr<MLGDao> m_dao;
};

} // end namespace mld

#endif // MLD_ABSTRACTOPERATOR_H
