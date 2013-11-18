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

#include "mld/MLGBuilder.h"
#include "mld/operator/AbstractOperator.h"

using namespace mld;

MLGBuilder::MLGBuilder()
{
}

MLGBuilder::~MLGBuilder()
{
}

bool MLGBuilder::run()
{
    if( m_steps.empty() ) {
        LOG(logWARNING) << "MLGBuilder::run queue is empty";
        return false;
    }
    // Run each step and remove it from the queue
    while( !m_steps.empty() ) {
        OperatorPtr& step = m_steps.front();
        bool ok = step->run();
        if( !ok ) {
            LOG(logERROR) << "MLGBuilder::run: an operation failed, stop";
            clearSteps();
            return false;
        }
        m_steps.pop_front();
    }
    return true;
}
