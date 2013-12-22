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

#ifndef MLD_XCOARSENER_H
#define MLD_XCOARSENER_H

#include <dex/gdb/Objects.h>

#include "mld/common.h"
#include "mld/operator/AbstractCoarsener.h"

namespace mld {

class SuperNode;
class XSelector;

/**
 * @brief The XCoarsener class
 *  Special multi-coarsener with auto adjust layering
 */
class MLD_API XCoarsener: public AbstractOperator
{
    XCoarsener( dex::gdb::Graph* g );
    virtual ~XCoarsener() override;

protected:
    virtual bool preExec() override;
    virtual bool exec() override;
    virtual bool postExec() override;

private:
    /**
     * @brief Create HLINKS for newly created top root node
     * @param root root SuperNode vlinked to rootTop
     * @param rootTop Top root SuperNode vlinked to root
     * @return success
     */
    bool createHLinksTopLayer( const SuperNode& root, const SuperNode& rootTop );

    /**
     * @brief Update or create HLink between 2 top layer root nodes
     * Merge 2 hlinks if link already exists
     *
     *                  HLink
     *      rootTop ---------- currentRootTop
     *     /                  /
     *    /                  /
     *   /                  / VLink
     *  /          *       /
     * /           |      /
     * root - n* - n2 - *r2 (3hop root node)
     *             |
     *             *
     *
     * @param currentNode 1hop or 2-hop current node (n2)
     * @param currentNeighbors 1 or 2-hop current neighbors (*)
     * @param rootNeighbors 1-hop root node neighbors (n)
     * @param rootTop Corresponding top layer root node
     * @param currentRootTop 2 or 3-hop root top layer node (r2)
     * @return success
     */
    virtual bool updateOrCreateHLink( dex::gdb::oid_t currentNode,
                                      ObjectsPtr& currentNeighbors,
                                      ObjectsPtr& rootNeighbors,
                                      const SuperNode& rootTop,
                                      const SuperNode& currentRootTop );

protected:
    std::unique_ptr<XSelector> m_sel;
    std::unique_ptr<AbstractMultiMerger> m_merger;
};


} // end namespace mld

#endif // XCOARSENER_H
