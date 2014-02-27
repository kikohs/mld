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

#ifndef MLD_MULTIADDITIVEMERGER_H
#define MLD_MULTIADDITIVEMERGER_H

#include "mld/common.h"
#include "mld/operator/mergers/AbstractMerger.h"

namespace mld {

class MLD_API MultiAdditiveMerger : public AbstractMultiMerger
{
public:
    MultiAdditiveMerger( sparksee::gdb::Graph* g );
    virtual ~MultiAdditiveMerger() override;

    virtual double computeWeight( const SuperNode& target, const ObjectsPtr& neighbors ) override;
    virtual bool merge( SuperNode& target, const ObjectsPtr& neighbors ) override;
};

} // end namespace mld

#endif // MULTIADDITIVEMERGER_H
