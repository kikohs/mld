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

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
}}

namespace mld {

class MLGDao;
class AbstractFilter;

class MLD_API TSOperator : public AbstractOperator
{
public:
    TSOperator( sparksee::gdb::Graph* g );
    virtual ~TSOperator() override;

    /**
     * @brief Set filter in the operator and TAKE OWNERSHIP of it
     * @param filter
     */
    void setFilter( AbstractFilter* filter );

    /**
     * @brief Name of this operator, use underlying name filter if any
     * @return name
     */
    virtual std::string name() const;

    /**
     * @brief Define nodes which will be processed
     * By default all the base layer is processed
     * @param nodeSet
     */
    void setActiveNodes( const ObjectsPtr& nodeSet );

    /**
     * @brief Define layers (time step in our case) where
     * data will be retrieve.
     * By default takes on the layers
     * @param layerSet
     */
    void setActiveLayers( const ObjectsPtr& layerSet );

    /**
     * @brief Exclude Nodes, they will not
     * be processed nor retrieved as neighbors
     * @param nodeSet
     */
    void setExcludedNodes( const ObjectsPtr& nodeSet );

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
    std::unique_ptr<MLGDao> m_dao;
    std::unique_ptr<AbstractFilter> m_filt;

    ObjectsPtr m_activeLayers;
    ObjectsPtr m_activeNodes;
    ObjectsPtr m_excludedNodes;
};

} // end namespace mld

#endif // TSOPERATOR_H
