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
    virtual ObjectsPtr getUnflaggedNeighbors( dex::gdb::oid_t node ) override;
    virtual bool flagAndUpdateScore( const SuperNode& root, bool withFlagged=false ) override;
    virtual bool isFlagged( dex::gdb::oid_t snid ) override;

    virtual ObjectsPtr getFlaggedNodesFrom( const ObjectsPtr& input ) override;
    virtual ObjectsPtr getUnflaggedNodesFrom( const ObjectsPtr& input ) override;

    /**
     * @brief Score function
     * @param snid SuperNode oid
     * @return score
     */
    virtual double calcScore( dex::gdb::oid_t snid, bool withFlagged );

    /**
     * @brief Update node score from given input set
     * @param input set
     * @return success
     */
    virtual bool updateScore( const ObjectsPtr& input, bool withFlagged );

    dex::gdb::Objects* rootNodes() const { return m_rootNodes.get(); }
    dex::gdb::Objects* flaggedNodes() const { return m_flagged.get(); }

    /**
     * @brief Remove input from selection queue
     * @param input
     */
    virtual void removeCandidates( const ObjectsPtr& input );

    /**
     * @brief Remaining SuperNode ids in the selection queue
     * @return nodes id
     */
    ObjectsPtr remainingNodes();

    // Score related functions, pure functions
    double rootCentralityScore( dex::gdb::oid_t node, bool withFlagged );
    double twoHopHubAffinityScore( dex::gdb::oid_t node, bool withFlagged );
    double gravityScore( dex::gdb::oid_t node, bool withFlagged );
    double getEdgeWeight( const ObjectsPtr& edgeOids );
    ObjectsPtr outEdges( dex::gdb::oid_t root, bool withFlagged );
    ObjectsPtr inEdges( dex::gdb::oid_t root, bool withFlagged );
    /**
     * @brief inOrOutEdges, retrieve all edges within 1hop radius or all out edges from 1 hop
     * radius
     * @param inEdges, if true get inedges, else out edges
     * @param root node
     * @param dao
     * @return edge set
     */
    ObjectsPtr inOrOutEdges( bool inEdges, dex::gdb::oid_t root, bool withFlagged );
    void edgeRetriever( ObjectsPtr& edgeSet, dex::gdb::oid_t source, const ObjectsPtr& targetSet );

protected:
    dex::gdb::oid_t m_lid;
    ObjectsPtr m_flagged;
    ObjectsPtr m_rootNodes;
    mutable_priority_queue<double, dex::gdb::oid_t> m_scores;
};

} // end namespace mld

#endif // XSELECTOR_H
