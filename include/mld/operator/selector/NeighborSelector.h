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

namespace mld {

/**
 * @brief The NeighborSelector abstract class
 */
class NeighborSelector : public AbstractSelector
{
public:
    NeighborSelector( sparksee::gdb::Graph* g );
    virtual ~NeighborSelector();

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

protected:
    /**
     * @brief Update node score from given input set
     * @param input set
     * @return success
     */
    virtual bool updateScore( const ObjectsPtr& input );

protected:
    bool m_withMemory;
    sparksee::gdb::oid_t m_current;
    ObjectsPtr m_curNeighbors;
};

} // end namespace mld

#endif // MLD_NEIGHBORSELECTOR_H
