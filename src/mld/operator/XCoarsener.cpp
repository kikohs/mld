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

#include "mld/operator/XCoarsener.h"
#include "mld/operator/XSelector.h"
#include "mld/operator/MultiAdditiveMerger.h"

using namespace mld;
using namespace dex::gdb;

XCoarsener::XCoarsener( Graph* g )
    : AbstractOperator(g)
    , m_sel( new XSelector(g) )
    , m_merger( new MultiAdditiveMerger(g) )
{
}

XCoarsener::~XCoarsener()
{
}

bool XCoarsener::pre_exec()
{
    // TODO
    return false;
}

bool XCoarsener::exec()
{
    // TODO
    return false;
}

bool XCoarsener::post_exec()
{
    // TODO
    return false;
}
