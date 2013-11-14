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

#ifndef MLD_COARSENERS_H
#define MLD_COARSENERS_H

#include "mld/common.h"
#include "mld/operator/AbstractCoarsener.h"

namespace mld {

/**
 * @brief The HeavyEdgeCoarsener class
 *  Mirrors the top layer
 *  Iteratively get the heaviest edge in the top layer and merge
 *  the 2 nodes until goal is reached
 */
class MLD_API HeavyEdgeCoarsener: public AbstractCoarsener
{
public:
    HeavyEdgeCoarsener( dex::gdb::Graph* g );
    virtual ~HeavyEdgeCoarsener() override;
};

} // end namespace mld

#endif // MLD_COARSENERS_H
