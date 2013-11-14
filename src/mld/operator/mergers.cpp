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

#include "mld/operator/mergers.h"

using namespace mld;

BasicAdditiveMerger::BasicAdditiveMerger( dex::gdb::Graph* g )
    : AbstractMerger(g)
{
}

BasicAdditiveMerger::~BasicAdditiveMerger()
{
}

bool BasicAdditiveMerger::merge( const HLink& hlink, const AbstractSelector& selector )
{
    return false;
}
