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

#ifndef MLD_ABSTRACTSELECTOR_H
#define MLD_ABSTRACTSELECTOR_H

#include "mld/common.h"
#include "mld/model/Link.h"
#include "mld/model/Layer.h"
#include "mld/model/SuperNode.h"

namespace sparksee {
namespace gdb {
    class Graph;
}}
namespace mld {
// Forward declaration
class MLGDao;

/**
 * @brief Base class inherited by all edge selectors
 * Provides a common interface for all mergers.
 * Used by all coarseners
 */
class MLD_API AbstractSelector
{
public:
    AbstractSelector( sparksee::gdb::Graph* g );
    virtual ~AbstractSelector() = 0;

    // Disable copy and assignement ctor
    AbstractSelector( const AbstractSelector& ) = delete;
    AbstractSelector& operator=( const AbstractSelector& ) = delete;

    /**
     * @brief Selector's name
     * @return
     */
    virtual std::string name() const = 0;

protected:
    std::unique_ptr<MLGDao> m_dao;
};

// Single edge selector
class MLD_API AbstractSingleSelector: public AbstractSelector
{
public:
    AbstractSingleSelector( sparksee::gdb::Graph* g );
    virtual ~AbstractSingleSelector() = 0;
    virtual HLink selectBestHLink( const Layer& layer ) = 0;
};

// Multi edge selector
class MLD_API AbstractMultiSelector: public AbstractSelector
{
public:
    AbstractMultiSelector( sparksee::gdb::Graph* g );
    virtual ~AbstractMultiSelector() = 0;

    /**
     * @brief Give a score to each node for coarsening
     * @param layer input layer
     * @return success
     */
    virtual bool rankNodes( const Layer& layer ) = 0;

    /**
     * @brief Has available supernode to process
     * @return SuperNode availability
     */
    virtual bool hasNext() = 0;

    /**
     * @brief Get best node according to score, consume node
     * @param popNode remove node from selection queue
     * @return Best supernode
     */
    virtual SuperNode next( bool popNode=true ) = 0;

    /**
     * @brief Get neighborhood of input node which is not already marked as unavailable
     * @param node
     * @return SuperNode oids
     */
    virtual ObjectsPtr getUnflaggedNeighbors( sparksee::gdb::oid_t node ) = 0;

    /**
     * @brief Flag node as processed, update 2-hop neighbors score
     * @param node Input node
     * @param removeNeighbors, if true neighbors are removes from selection list
     * @param withFlagged Account for already flagged nodes in score
     * @return success
     */
    virtual bool flagAndUpdateScore( const SuperNode& root, bool removeNeighbors=true, bool withFlagged=false ) = 0;
    /**
     * @brief Check if some input node oid is already in the flagged set
     * @param snid input
     * @return flagged
     */
    virtual bool isFlagged( sparksee::gdb::oid_t snid ) = 0;

    /**
     * @brief Filter input to return only flagged nodes
     * @param input
     * @return flagged nodes
     */
    virtual ObjectsPtr getFlaggedNodesFrom( const ObjectsPtr& input ) = 0;

    /**
     * @brief Filter input to return only non flagged nodes
     * @param input
     * @return non flagged nodes
     */
    virtual ObjectsPtr getUnflaggedNodesFrom( const ObjectsPtr& input ) = 0;

    /**
     * @brief Get endpoints (node ids) for current root node to create HLinks.
     * Get flagged 1 hop or unflagged 2-hop's flagged nodes and 1-hop flagged targets to create
     * HLinks
     * @param root Root node
     * @param oneHopOnly if true only the first hop will be retrieved
     * @return nodeSet
     */
    virtual ObjectsPtr getHLinkEnpoints( const SuperNode& root, bool oneHopOnly ) = 0;
};

} // end namespace mld

#endif // MLD_ABSTRACTSELECTOR_H
