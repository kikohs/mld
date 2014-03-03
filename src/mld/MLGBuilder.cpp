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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "mld/MLGBuilder.h"
#include "mld/operator/AbstractOperator.h"
#include "mld/operator/coarseners.h"
#include "mld/operator/selectors.h"
#include "mld/operator/mergers.h"
#include "mld/utils/Timer.h"

using namespace mld;
using namespace sparksee::gdb;
// Alias
namespace ba = boost::algorithm;

MLGBuilder::MLGBuilder()
{
}

MLGBuilder::~MLGBuilder()
{
    m_steps.clear();
}

bool MLGBuilder::run()
{
    LOG(logINFO) << "MLGBuilder::run";
    if( m_steps.empty() ) {
        LOG(logWARNING) << "MLGBuilder::run queue is empty";
        return false;
    }
    // Run each step and remove it from the queue
    while( !m_steps.empty() ) {
        CoarsenerPtr& step = m_steps.front();
        std::unique_ptr<Timer> t(new Timer("MLGBuilder::run step"));
        LOG(logINFO) << "MLGBuilder::run step " << *step;
        if( !step->run() ) {
            LOG(logERROR) << "MLGBuilder::run: an operation failed, stop";
            clearSteps();
            return false;
        }
        m_steps.pop_front();
    }
    return true;
}

bool MLGBuilder::fromRawString( Graph* g, const std::string& input )
{
    if( input.empty() ) {
        LOG(logERROR) << "MLGBuilder::fromRawString empty input";
        return false;
    }

    LOG(logINFO) << "MLGBuilder::fromRawString: " << input;
    std::vector<std::string> tokens;
    ba::split(tokens, input, boost::is_any_of(" "));

    // For each token
    float fac = 0.0;
    for( auto tok: tokens ) {
        std::vector<std::string> tokens2;
        // Read coarsener name left and right values
        ba::split(tokens2, tok, boost::is_any_of(":[,]"));
        if( tokens2.size() < 2 ) {
            LOG(logERROR) << "MLGBuilder::fromRawString parse error: " << input;
            m_steps.clear();
            return false;
        }
        std::string name(tokens2.at(0));
        float tmp = 0.0;
        for( size_t i = 1; i < tokens2.size(); ++i ) {
            // Last token shoulb be empty, we check that at least 1 tokens exists
            if( tokens2.at(i).empty() ) {
                if( tokens2.size() > 2 ) {
                    continue;
                }
                else {
                    LOG(logERROR) << "MLGBuilder::fromRawString parse error factor: " << tokens2[i];
                    m_steps.clear();
                    return false;
                }
            }

            try {
                tmp = boost::lexical_cast<float>(tokens2.at(i));
            }
            catch( const boost::bad_lexical_cast& ) {
                LOG(logERROR) << "MLGBuilder::fromRawString parse error factor: " << tokens2[i];
                m_steps.clear();
                return false;
            }
            // Add coarsening steps only if coarsening has a superior reduction fac
            if( tmp <= fac ) {
                LOG(logERROR) << "MLGBuilder::fromRawString parse error factor is inferior to last one: " << tokens2[i];
                m_steps.clear();
                return false;
            }

            fac = tmp;
            CoarsenerPtr coars(createCoarsener(g, name, fac));
            if( !coars ) {
                m_steps.clear();
                return false;
            }

            // Every test has passed
            m_steps.push_back(coars);
        }
    }
    return true;
}

CoarsenerPtr MLGBuilder::createCoarsener( Graph* g, const std::string& name, float fac )
{
    auto* res = new NeighborCoarsener(g);
    res->setMerger( new AdditiveNeighborMerger(g) );
    res->setReductionFactor(fac);

    // Set selector
    if( name == "Hs" ) {
        res->setSelector( new HeavyHLinkSelector(g) );
    }
    else if( name == "Hm" ) {
        auto* sel = new HeavyHLinkSelector(g);
        sel->setHasMemory(true);
        res->setSelector(sel);
    }
    else if ( name == "Xs") {
        res->setSelector( new XSelector(g) );
    }
    else if ( name == "Xm") {
        auto* sel = new XSelector(g);
        sel->setHasMemory(true);
        res->setSelector(sel);
    }
    else {
        LOG(logERROR) << "MLGBuilder::createCoarsener unsupported coarsener";
        delete res;
        return CoarsenerPtr();
    }
    return CoarsenerPtr(res);
}
