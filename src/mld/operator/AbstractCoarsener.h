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

#ifndef MLD_ABSTRACTCOARSENER_H
#define MLD_ABSTRACTCOARSENER_H

#include "mld/common.h"
#include "mld/operator/AbstractOperator.h"

namespace mld {

class AbstractSelector;
class AbstractMerger;

class MLD_API AbstractCoarsener : public AbstractOperator
{
public:
    AbstractCoarsener( dex::gdb::Graph* g );
    virtual ~AbstractCoarsener() = 0;

    /**
     * @brief Scale Factor, for coarsener or expanders
     *  The factor is always calculated from the Base layer
     * E.g 0.9 means that the goal is to keep 90% of base layer nodes
     * @param fac
     */
    void setScaleFactor( float fac ) { m_scaleFac = fac; }
    float scaleFactor() const { return m_scaleFac; }

protected:
    virtual void pre_exec() override;
    virtual void exec() override;
    virtual void post_exec() override;

protected:
    bool m_success;
    float m_scaleFac;
    std::unique_ptr<AbstractSelector> m_sel;
    std::unique_ptr<AbstractMerger> m_merger;
};

} // end namespace mld

#endif // MLD_ABSTRACTCOARSENER_H
