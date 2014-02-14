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

#ifndef MLD_MERGERS_H
#define MLD_MERGERS_H

#include "mld/common.h"
#include "mld/operator/AbstractMerger.h"
#include "mld/operator/MultiAdditiveMerger.h"

namespace mld {

class MLD_API BasicAdditiveMerger: public AbstractSingleMerger
{
public:
    BasicAdditiveMerger( dex::gdb::Graph* g );
    virtual ~BasicAdditiveMerger() override;

    virtual double computeWeight( const SuperNode& source, const HLink& hlink ) override;
    virtual bool merge( const HLink& hlink, const AbstractSingleSelector& selector ) override;
};

} // end namespace mld

#endif // MLD_MERGERS_H
