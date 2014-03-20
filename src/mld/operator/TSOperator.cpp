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

#include <sparksee/gdb/Graph.h>
#include <sparksee/gdb/Objects.h>
#include <sparksee/gdb/ObjectsIterator.h>

#include "mld/operator/TSOperator.h"
#include "mld/dao/MLGDao.h"
#include "mld/operator/filter/AbstractFilter.h"
#include "mld/utils/Timer.h"
#include "mld/utils/ProgressDisplay.h"

using namespace mld;
using namespace sparksee::gdb;

TSOperator::TSOperator( Graph* g )
    : m_dao(new MLGDao(g))
    , m_filt(nullptr)
{
}

TSOperator::~TSOperator()
{
}

void TSOperator::setFilter( AbstractFilter* filter )
{
    m_filt.reset(filter);
}

std::string TSOperator::name() const
{
    // TODO
    return std::string();
}

void TSOperator::setActiveNodes( const ObjectsPtr& nodeSet )
{
    m_activeNodes = nodeSet;
}

void TSOperator::setActiveLayers( const ObjectsPtr& layerSet )
{
    m_activeLayers = layerSet;
}

void TSOperator::setExcludedNodes( const ObjectsPtr& nodeSet )
{
    m_excludedNodes = nodeSet;
}

bool TSOperator::preExec()
{
    if( !m_filt ) {
        LOG(logERROR) << "TSOperator::preExec: No filter set. Please set a filter first";
        return false;
    }

    // Select all nodes from base layer
    if( !m_activeNodes ) {
        m_activeNodes = m_dao->getAllNodeIds(m_dao->baseLayer());
    }

    // Set empty set
    if( !m_excludedNodes ) {
        m_excludedNodes = m_dao->newObjectsPtr();
    }

    // Select all layers
    if( !m_activeLayers ) {
        m_activeLayers.reset(m_dao->graph()->Select(m_dao->olinkType()));
    }
    // TODO
    return false;
}

bool TSOperator::exec()
{
    // TODO
    return false;
}

bool TSOperator::postExec()
{
    // TODO
    return false;
}
