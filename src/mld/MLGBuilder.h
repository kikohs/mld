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

namespace sparksee {
namespace gdb {
    class Graph;
}}

namespace mld {

class AbstractCoarsener;

typedef std::shared_ptr<AbstractCoarsener> CoarsenerPtr;
/**
 * @brief The Multi-layer graph builder class
 */
class MLD_API MLGBuilder
{
public:
    MLGBuilder();
    virtual ~MLGBuilder();
    MLGBuilder( const MLGBuilder& ) = delete;
    MLGBuilder& operator=( const MLGBuilder& ) = delete;

    /**
     * @brief Add step to the queue
     * @param step
     */
    void addStep( const CoarsenerPtr& step ) { m_steps.push_back(step); }
    void clearSteps() { m_steps.clear(); }

    /**
     * @brief Parse coarsening plan from input
     * Create corresponding coarseners
     * @param input
     * @return parsing success
     */
    bool fromRawString( sparksee::gdb::Graph* g, const std::string& input );

    /**
     * @brief Run all the steps FIFO, empyting the queue
     * @return success
     */
    bool run();

private:
    CoarsenerPtr createCoarsener( sparksee::gdb::Graph* g, const std::string& name, float fac );

private:
    std::deque<CoarsenerPtr> m_steps;
};

} // end namespace mld

#endif // MLD_MLGBUILDER_H

