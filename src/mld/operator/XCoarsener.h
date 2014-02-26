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

#include <sparksee/gdb/Objects.h>

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
    XCoarsener( sparksee::gdb::Graph* g );
    virtual ~XCoarsener() override;
    virtual std::string name() const override;

    /**
     * @brief The reduction factor is no longer a strict constraint
     * but a minimal bound on the layer.
     * Under the hood it uses the multipass if true
     * @param v
     */
    void setReducFacAsStrictBound( bool v ) { m_strictBound = v; }

protected:
    virtual bool preExec() override;
    virtual bool exec() override;
    virtual bool postExec() override;

private:
    /**
     * @brief Do the coarsening in 1 pass by mirroring the current layer
     * @return sucess
     */
    bool singlePass( const Layer& top, int64_t& mergeCount );

    /**
     * @brief Do coarsening in 2 pass, creating top layer on the fly
     * @return
     */
    bool multiPass( const Layer& prev, const Layer& top, int64_t& mergeCount );

    /**
     * @brief First pass for the coarsening. Add SuperNodes on top layer
     * @param top
     * @param mergeCount
     * @return success
     */
    bool currentLayerPass( const Layer& prev, const Layer& top, int64_t& mergeCount );

    /**
     * @brief Second which coarsens the toplayer until merge count condition is reached
     * @param top
     * @param mergeCount
     * @return success
     */
    bool topLayerPass( const Layer& top, int64_t& mergeCount );

    /**
     * @brief If the coarsening stops before the all layer is coarsened
     * the remaining uncoarsened nodes need to be mirrored
     * @param top Layer
     * @return success
     */
    bool mirrorRemainingNodes( const Layer& top );

protected:
    std::unique_ptr<XSelector> m_sel;
    std::unique_ptr<AbstractMultiMerger> m_merger;
    bool m_strictBound;
};


} // end namespace mld

#endif // XCOARSENER_H
