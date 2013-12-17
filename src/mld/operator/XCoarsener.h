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

#include "mld/common.h"
#include "mld/operator/AbstractCoarsener.h"

namespace mld {

class SuperNode;

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
     */
    void createHLinksTopLayer( const SuperNode& root, const SuperNode& rootTop );

protected:
    std::unique_ptr<AbstractMultiSelector> m_sel;
    std::unique_ptr<AbstractMultiMerger> m_merger;
};


} // end namespace mld

#endif // XCOARSENER_H
