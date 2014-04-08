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

#ifndef MLD_TSOPERATOR_H
#define MLD_TSOPERATOR_H

#include "mld/operator/AbstractOperator.h"
#include "mld/model/Link.h"
#include "mld/operator/TSCache.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
}}

namespace mld {

class MLGDao;
class AbstractTimeVertexFilter;

class MLD_API TSOperator : public AbstractOperator
{
public:
    TSOperator( sparksee::gdb::Graph* g );
    virtual ~TSOperator() override;

    /**
     * @brief Set filter in the operator and TAKE OWNERSHIP of it
     * @param filter
     */
    void setFilter( AbstractTimeVertexFilter* filter );

protected:
    /**
     * @brief Select set of Nodes to operate
     * @return success
     */
    virtual bool preExec() override;
    /**
     * @brief Execute the filter on each node selected
     * @return success
     */
    virtual bool exec() override;
    /**
     * @brief Commit the new values in the corresponding OLink
     * @return success
     */
    virtual bool postExec() override;

protected:
    std::shared_ptr<MLGDao> m_dao;
    std::shared_ptr<TSCache> m_cache;
    std::unique_ptr<AbstractTimeVertexFilter> m_filt;
    std::vector<OLink> m_buffer; // store OLink to be commited
};

} // end namespace mld

#endif // TSOPERATOR_H
