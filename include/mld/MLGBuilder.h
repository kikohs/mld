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

#ifndef MLD_MLGBUILDER_H
#define MLD_MLGBUILDER_H

#include <deque>

#include "mld/common.h"

namespace mld {

class AbstractOperator;

typedef std::shared_ptr<AbstractOperator> OperatorPtr;
/**
 * @brief The Multi-layer graph builder class
 */
class MLD_API MLGBuilder
{
public:
    MLGBuilder();
    MLGBuilder( const MLGBuilder& ) = delete;
    MLGBuilder& operator=( const MLGBuilder& ) = delete;

    /**
     * @brief Add step to the queue
     * @param step
     */
    void addStep( const OperatorPtr& step ) { m_steps.push_back(step); }
    void clearSteps() { m_steps.clear(); }

    /**
     * @brief Run all the steps FIFO, empyting the queue
     */
    void run();
private:
    std::deque<OperatorPtr> m_steps;
};

} // end namespace mld

#endif // MLD_MLGBUILDER_H

