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

#ifndef MLD_ABSTRACTMERGER_H
#define MLD_ABSTRACTMERGER_H

#include "mld/common.h"

namespace dex {
namespace gdb {
    class Graph;
}}

namespace mld {
// Forward declaration
class MLGDao;
class HLink;
class SuperNode;
class AbstractSingleSelector;
class AbstractMultiSelector;

/**
 * @brief Base class inherited by all edge mergers
 * Used by all coarseners
 */
class MLD_API AbstractMerger
{
public:
    AbstractMerger( dex::gdb::Graph* g );
    virtual ~AbstractMerger() = 0;

    // Disable copy and assignement ctor
    AbstractMerger( const AbstractMerger& ) = delete;
    AbstractMerger& operator=( const AbstractMerger& ) = delete;
protected:
    std::unique_ptr<MLGDao> m_dao;
};

// Single Merger
class MLD_API AbstractSingleMerger: public AbstractMerger
{
public:
    AbstractSingleMerger( dex::gdb::Graph* g );
    virtual ~AbstractSingleMerger() = 0;

    /**
     * @brief Collapse a HLink depending on the underlying algorithm
     * Update graph and selector
     * @param hlink
     * @param selector
     * @return success
     */
    virtual bool merge( const HLink& hlink, const AbstractSingleSelector& selector ) = 0;

    /**
     * @brief Compute weight for the future merged node
     * @param target Node to update
     * @param hlink Hlink
     * @return new weight
     */
    virtual double computeWeight( const SuperNode& target, const HLink& hlink ) = 0;
};

// Multi Merger
class MLD_API AbstractMultiMerger: public AbstractMerger
{
public:
    AbstractMultiMerger( dex::gdb::Graph* g );
    virtual ~AbstractMultiMerger() = 0;

    /**
     * @brief Collapse all available HLinks and node around a node depending on the underlying algorithm
     * Update graph and selector
     * @param source node
     * @param neighbors Available neighbors to collapse
     * @return success
     */
    virtual bool merge( const SuperNode& source, const ObjectsPtr& neighbors ) = 0;

    /**
     * @brief Compute weight for the future merged node
     * @param target Node to update
     * @param neighbors Neighbors of root node
     * @return new weight
     */
    virtual double computeWeight( const SuperNode& target, const ObjectsPtr& neighbors ) = 0;
};


} // end namespace mld

#endif // MLD_ABSTRACTMERGER_H
