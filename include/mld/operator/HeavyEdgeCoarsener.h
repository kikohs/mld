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

#ifndef MLD_HEAVYEDGECOARSENER_H
#define MLD_HEAVYEDGECOARSENER_H

#include "mld/common.h"
#include "mld/operator/AbstractOperator.h"

namespace mld {

class MLD_API HeavyEdgeCoarsener: public AbstractOperator
{
public:
    HeavyEdgeCoarsener( dex::gdb::Graph* g );
    virtual ~HeavyEdgeCoarsener() override;

protected:
    virtual void pre_exec() override;
    virtual void exec() override;
    virtual void post_exec() override;

private:
    bool m_success;
};

} // end namespace mld

#endif // MLD_HEAVYEDGECOARSENER_H
