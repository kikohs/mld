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

#ifndef MLD_NEIGHBORCOARSENER_H
#define MLD_NEIGHBORCOARSENER_H

#include "mld/operator/coarsener/AbstractCoarsener.h"

namespace mld {

class NeighborCoarsener : public AbstractCoarsener
{
public:
    NeighborCoarsener( sparksee::gdb::Graph* g );
    virtual ~NeighborCoarsener();

    virtual std::string name() const override;

    /**
     * @brief Set selector and TAKE OWNERSHIP
     * @param sel
     */
    void setSelector( NeighborSelector* sel ) { m_sel.reset(sel); }
    /**
     * @brief Set merger and TAKE OWNERSHIP
     * @param merger
     */
    void setMerger( NeighborMerger* merger ) { m_merger.reset(merger); }

protected:
    virtual bool preExec() override;
    virtual bool exec() override;
    virtual bool postExec() override;

protected:
    std::unique_ptr<NeighborSelector> m_sel;
    std::unique_ptr<NeighborMerger> m_merger;
};

} // end namespace mld

#endif // MLD_NEIGHBORCOARSENER_H
