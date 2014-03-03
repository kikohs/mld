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

#include "mld/operator/selector/NeighborSelector.h"

namespace mld {

class MLD_API XSelector : public NeighborSelector
{
public:
    XSelector( sparksee::gdb::Graph* g );
    virtual ~XSelector() override;

    virtual std::string name() const override { return "XSelector"; }

    /**
     * @brief Score function
     * @param snid SuperNode oid
     * @return score
     */
    virtual double calcScore( sparksee::gdb::oid_t snid ) override;

    // Score related functions, pure functions
    double rootCentralityScore( sparksee::gdb::oid_t node );
    double twoHopHubAffinityScore( sparksee::gdb::oid_t node );
    double gravityScore( sparksee::gdb::oid_t node );
    double getEdgeWeight( const ObjectsPtr& edgeOids );
    ObjectsPtr outEdges( sparksee::gdb::oid_t root );
    ObjectsPtr inEdges( sparksee::gdb::oid_t root );
    /**
     * @brief inOrOutEdges, retrieve all edges within 1hop radius or all out edges from 1 hop
     * radius
     * @param inEdges, if true get inedges, else out edges
     * @param root node
     * @param dao
     * @return edge set
     */
    ObjectsPtr inOrOutEdges( bool inEdges, sparksee::gdb::oid_t root );
    void edgeRetriever( ObjectsPtr& edgeSet, sparksee::gdb::oid_t source, const ObjectsPtr& targetSet );

protected:
    /**
     * @brief Set current Neighbor with the heaviest HLink endpoint
     */
    virtual void setCurrentBestNeighbors() override;
};

} // end namespace mld

#endif // XSELECTOR_H
