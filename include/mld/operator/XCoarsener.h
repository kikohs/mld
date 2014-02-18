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
#include "mld/model/Layer.h"

namespace mld {

class SuperNode;
class XSelector;

/**
 * @brief The XCoarsener class
 *  Special multi-coarsener with auto adjust layering
 */
class MLD_API XCoarsener: public AbstractCoarsener
{
public:
    XCoarsener( dex::gdb::Graph* g );
    virtual ~XCoarsener() override;

protected:
    virtual bool preExec() override;
    virtual bool exec() override;
    virtual bool postExec() override;

private:

    /**
     * @brief First pass for the coarsening. Add SuperNodes on top layer
     * @param top
     * @param mergeCount
     * @return success
     */
    bool firstPass( const Layer& prev, const Layer& top, int64_t& mergeCount );

    /**
     * @brief Second which coarsens the toplayer until merge count condition is reached
     * @param top
     * @param mergeCount
     * @return success
     */
    bool secondPass( const Layer& top, int64_t& mergeCount );

    /**
     * @brief If the coarsening stops before the all layer is coarsened
     * the remaining uncoarsened nodes need to be mirrored
     * @param nodes
     * @return success
     */
    bool mirrorRemainingNodes( const Layer& top, const ObjectsPtr& nodes );

    /**
     * @brief Create HLINKS for newly created top root node
     * The search radius is 1 hop for the current root node
     * @param root root SuperNode vlinked to rootTop
     * @param rootTop Top root SuperNode vlinked to root
     * @return success
     */
    bool createTopHLinks1Hop( const SuperNode& root, const SuperNode& rootTop );

    /**
     * @brief Create HLINKS for newly created top root node
     * The search radius is 2 hop, 1 hop for the current root node
     * and 1 hop for each the root's neighbors
     *                HLink
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
     * @param root root SuperNode vlinked to rootTop
     * @param rootTop Top root SuperNode vlinked to root
     * @return success
     */
    bool createTopHLinks1N2Hops( const SuperNode& root, const SuperNode& rootTop );

    bool createTopHLinks( const SuperNode& root, const SuperNode& rootTop, const ObjectsPtr& nodeSet );

protected:
    std::unique_ptr<XSelector> m_sel;
    std::unique_ptr<AbstractMultiMerger> m_merger;
};


} // end namespace mld

#endif // XCOARSENER_H
