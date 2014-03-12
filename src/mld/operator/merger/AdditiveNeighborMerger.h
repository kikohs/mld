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

#ifndef MLD_ADDITIVENEIGHBORMERGER_H
#define MLD_ADDITIVENEIGHBORMERGER_H

#include "mld/operator/merger/NeighborMerger.h"

namespace mld {

/**
 * @brief The AdditiveNeighborMerger class
 * Add weights for common edges
 */
class MLD_API AdditiveNeighborMerger : public NeighborMerger
{
public:
    AdditiveNeighborMerger( sparksee::gdb::Graph* g );
    virtual ~AdditiveNeighborMerger() override;
    virtual bool merge( Node& target, const ObjectsPtr& neighbors ) override;
    virtual double computeWeight( const Node& target, const ObjectsPtr& neighbors ) override;
    virtual std::string name() const override { return "AdditiveNeighborMerger"; }
};

} // end namespace mld

#endif // MLD_ADDITIVENEIGHBORMERGER_H
