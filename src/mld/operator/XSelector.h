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
#include <sparksee/gdb/Objects.h>

#include "mld/common.h"
#include "mld/operator/AbstractSelector.h"
#include "mld/utils/mutable_priority_queue.h"

namespace mld {

class MLD_API XSelector : public AbstractMultiSelector
{
public:
    XSelector( sparksee::gdb::Graph* g );
    virtual ~XSelector() override;

    virtual bool rankNodes( const Layer& layer ) override;
    virtual bool hasNext() override;
    virtual SuperNode next( bool popNode=true ) override;
    virtual ObjectsPtr getUnflaggedNeighbors( sparksee::gdb::oid_t node ) override;
    virtual bool flagAndUpdateScore( const SuperNode& root, bool removeNeighbors=true, bool withFlagged=false ) override;
    virtual bool isFlagged( sparksee::gdb::oid_t snid ) override;

    virtual ObjectsPtr getFlaggedNodesFrom( const ObjectsPtr& input ) override;
    virtual ObjectsPtr getUnflaggedNodesFrom( const ObjectsPtr& input ) override;

    /**
     * @brief Score function
     * @param snid SuperNode oid
     * @return score
     */
    virtual double calcScore( sparksee::gdb::oid_t snid, bool withFlagged );

    /**
     * @brief Update node score from given input set
     * @param input set
     * @return success
     */
    virtual bool updateScore( const ObjectsPtr& input, bool withFlagged );

    virtual ObjectsPtr getHLinkEnpoints( const SuperNode& root, bool oneHopOnly ) override;

    sparksee::gdb::Objects* rootNodes() const { return m_rootNodes.get(); }
    sparksee::gdb::Objects* flaggedNodes() const { return m_flagged.get(); }

    /**
     * @brief Remove input from selection queue
     * @param input
     */
    virtual void removeCandidates( const ObjectsPtr& input );

    // Score related functions, pure functions
    double rootCentralityScore( sparksee::gdb::oid_t node, bool withFlagged );
    double twoHopHubAffinityScore( sparksee::gdb::oid_t node, bool withFlagged );
    double gravityScore( sparksee::gdb::oid_t node, bool withFlagged );
    double getEdgeWeight( const ObjectsPtr& edgeOids );
    ObjectsPtr outEdges( sparksee::gdb::oid_t root, bool withFlagged );
    ObjectsPtr inEdges( sparksee::gdb::oid_t root, bool withFlagged );
    /**
     * @brief inOrOutEdges, retrieve all edges within 1hop radius or all out edges from 1 hop
     * radius
     * @param inEdges, if true get inedges, else out edges
     * @param root node
     * @param dao
     * @return edge set
     */
    ObjectsPtr inOrOutEdges( bool inEdges, sparksee::gdb::oid_t root, bool withFlagged );
    void edgeRetriever( ObjectsPtr& edgeSet, sparksee::gdb::oid_t source, const ObjectsPtr& targetSet );

protected:
    sparksee::gdb::oid_t m_lid;
    ObjectsPtr m_flagged;
    ObjectsPtr m_rootNodes;
    mutable_priority_queue<double, sparksee::gdb::oid_t> m_scores;
};

} // end namespace mld

#endif // XSELECTOR_H
