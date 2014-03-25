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

#include "mld/operator/filter/AbstractVertexFilter.h"
#include "mld/dao/MLGDao.h"

using namespace mld;
using namespace sparksee::gdb;

AbstractVertexFilter::AbstractVertexFilter( Graph* g )
    : m_dao( new MLGDao(g) )
    , m_timeWindowSize(1)
    , m_override(false)
    , m_lambda(0.0)
    , m_excludedNodes(m_dao->newObjectsPtr())
{
}

AbstractVertexFilter::~AbstractVertexFilter()
{
}

void AbstractVertexFilter::setExcludedNodes( const ObjectsPtr& nodeSet )
{
    m_excludedNodes = nodeSet;
}

void AbstractVertexFilter::overrideInterLayerWeight( double w )
{
    m_override = true;
    m_lambda = w;
}

void AbstractVertexFilter::setTimeWindowSize( uint32_t nbHops )
{
    m_timeWindowSize = nbHops;
}
