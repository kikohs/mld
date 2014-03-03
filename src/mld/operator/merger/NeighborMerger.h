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

#ifndef MLD_NEIGHBORMERGER_H
#define MLD_NEIGHBORMERGER_H

#include "mld/operator/merger/AbstractMerger.h"

namespace mld {

class MLD_API NeighborMerger: public AbstractMerger
{
public:
    NeighborMerger( sparksee::gdb::Graph* g );
    virtual ~NeighborMerger() = 0;

    /**
     * @brief Collapse all available HLinks and node around a node depending on the underlying algorithm
     * Update graph and selector
     * @param target node
     * @param neighbors Available neighbors to collapse
     * @return success
     */
    virtual bool merge( SuperNode& target, const ObjectsPtr& neighbors ) = 0;

    /**
     * @brief Compute weight for the future merged node
     * @param target Node to update
     * @param neighbors Neighbors of root node
     * @return new weight
     */
    virtual double computeWeight( const SuperNode& target, const ObjectsPtr& neighbors ) = 0;
};

} // end namespace mld

#endif // MLD_NEIGHBORMERGER_H
