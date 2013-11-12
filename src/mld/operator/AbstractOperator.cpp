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

#include "mld/operator/AbstractOperator.h"
#include "mld/dao/MLGDao.h"

using namespace mld;

AbstractOperator::AbstractOperator( dex::gdb::Graph* g )
    : m_g(g)
    , m_dao( new MLGDao(g) )
    , m_scaleFac(1.0)
{
    MLD_UNUSED(m_g);
}

AbstractOperator::~AbstractOperator()
{
    // DO NOT DELETE GRAPH
}

void AbstractOperator::run()
{
    pre_exec();
    exec();
    post_exec();
}


