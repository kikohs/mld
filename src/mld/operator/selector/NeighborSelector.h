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

#ifndef MLD_NEIGHBORSELECTOR_H
#define MLD_NEIGHBORSELECTOR_H

#include "mld/operator/selector/AbstractSelector.h"
#include "mld/utils/mutable_priority_queue.h"

namespace mld {

/**
 * @brief The NeighborSelector abstract class
 */
class MLD_API NeighborSelector : public AbstractSelector
{
public:
    NeighborSelector( sparksee::gdb::Graph* g );
    virtual ~NeighborSelector();

    void setHasMemory( bool v ) { m_hasMemory = v; }
    bool hasMemory() const { return m_hasMemory; }
    /**
     * @brief Give a score to each node for coarsening
     * @param layer input layer
     * @return success
     */
    virtual bool rankNodes( const Layer& layer );

    /**
     * @brief Has available supernode to process
     * @return SuperNode availability
     */
    virtual bool hasNext();

    /**
     * @brief Get best node according to score, consume node
     * @return Best supernode
     */
    virtual SuperNode next();

    /**
     * @brief Score function, must be reimplemented in child classes
     * @param snid SuperNode oid
     * @return score
     */
    virtual double calcScore( sparksee::gdb::oid_t snid ) = 0;

    /**
     * @brief Get best neighbors for current selected node
     * @return Best neighbors
     */
    ObjectsPtr getCurrentBestNeighbors() const { return m_curNeighbors; }

    /**
     * @brief Get all flagged nodes by the selector
     * @return Flagged nodes
     */
    ObjectsPtr getFlaggedNodes() const { return m_flagged; }

protected:
    /**
     * @brief Set current best neighbors pure function to reimplement
     * m_current holds the current node, this function should set the
     * m_curNeighbors variable with the proper node set
     */
    virtual void setCurrentBestNeighbors() = 0;
    /**
     * @brief Update node score from given input set
     * @param input set
     * @return success
     */
    virtual bool updateScore( const ObjectsPtr& input );
    /**
     * @brief Remove already merged nodes
     * @param input shoudl be currentBestNeighbors
     */
    virtual void removeCandidates( const ObjectsPtr& input );

    /**
     * @brief Get neighbors for current node, with or without memory
     * @param snid
     * @return neighbors
     */
    ObjectsPtr getNeighbors( sparksee::gdb::oid_t snid );

protected:
    bool m_hasMemory;
    sparksee::gdb::oid_t m_layerId;  // Layer id
    sparksee::gdb::oid_t m_current;  // Current SuperNode id
    ObjectsPtr m_flagged; // Flagged node if memory
    ObjectsPtr m_curNeighbors;
    mutable_priority_queue<double, sparksee::gdb::oid_t> m_scores;
};

} // end namespace mld

#endif // MLD_NEIGHBORSELECTOR_H
