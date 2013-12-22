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

#ifndef MLD_XSELECTOR_H
#define MLD_XSELECTOR_H

#include <queue>
#include <dex/gdb/Objects.h>

#include "mld/common.h"
#include "mld/operator/AbstractSelector.h"
#include "mld/utils/mutable_priority_queue.h"

namespace mld {

class MLD_API XSelector : public AbstractMultiSelector
{
public:
    XSelector( dex::gdb::Graph* g );
    virtual ~XSelector() override;

    virtual bool rankNodes( const Layer& layer ) override;
    virtual bool hasNext() override;
    virtual SuperNode next() override;
    virtual ObjectsPtr unflaggedNeighbors( const SuperNode& node ) override;
    virtual bool flagNode( const SuperNode& root ) override;
    virtual bool isFlagged( dex::gdb::oid_t snid ) override;

    virtual ObjectsPtr flaggedNodes( const ObjectsPtr& input ) override;
    virtual ObjectsPtr unflaggedNodes( const ObjectsPtr& input ) override;

    /**
     * @brief Score function
     * @param snid SuperNode oid
     * @return score
     */
    virtual double calcScore( dex::gdb::oid_t snid );

    /**
     * @brief Update node score from given input set
     * @param input set
     * @return success
     */
    virtual bool updateScore( const ObjectsPtr& input );

    dex::gdb::Objects* rootNodes() const { return m_rootNodes.get(); }

    /**
     * @brief Remove input from selection queue
     * @param input
     */
    virtual void removeCandidates( const ObjectsPtr& input );

protected:
    dex::gdb::oid_t m_lid;
    ObjectsPtr m_flagged;
    ObjectsPtr m_rootNodes;
    mutable_priority_queue<double, dex::gdb::oid_t> m_scores;
};

} // end namespace mld

#endif // XSELECTOR_H
