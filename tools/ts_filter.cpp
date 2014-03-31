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

#include <locale>
#include <codecvt>
#include <string>

#include <boost/algorithm/string.hpp>
#include <tclap/CmdLine.h>

#include <mld/config.h>
#include <mld/SparkseeManager.h>
#include <mld/Session.h>
#include <mld/io/GraphImporter.h>
#include <mld/utils/Timer.h>
#include <mld/operator/TSOperator.h>
#include <mld/operator/filter/FilterFactory.h>

using namespace TCLAP;
using namespace mld;

const double INVALID_WEIGHT = -1.0;

struct InputContext {
    std::wstring dbName;
    std::wstring workDir;
    std::string filterName;
    double lambda;
    uint32_t twSize;
};

bool parseOptions( int argc, char *argv[], InputContext& out )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    try {
        // Define the command line object.
        CmdLine cmd("Filter", ' ', "0.1");

        // Working dir
        ValueArg<std::string> wdArg("d", "workDir", "MLD working directory",
                                    false, converter.to_bytes(mld::kRESOURCES_DIR), "path");
        cmd.add(wdArg);

        // Db Name
        ValueArg<std::string> nameArg("n", "name", "MLD database name (without extension)",
                                    true, "", "string");
        cmd.add(nameArg);

        // Filter name
        ValueArg<std::string> filterNameArg("f", "filter",
                                       "Filter\n TimeVertexMeanFilter",
                                       false, "tvm", "string");
        cmd.add(filterNameArg);

        // Lambda
        ValueArg<double> lambdaArg("l", "lambda", "InterLayer weight\n ", false, INVALID_WEIGHT, "double");
        cmd.add(lambdaArg);

        // TimwWindow size
        ValueArg<uint32_t> twSizeArg("s", "twSize", "TimeWindow size\n ", false, 1, "uint32_t");
        cmd.add(twSizeArg);

        // Parse the args.
        cmd.parse(argc, argv);

        // Get the value parsed by each arg.
        out.workDir = converter.from_bytes(wdArg.getValue());
        out.dbName = converter.from_bytes(nameArg.getValue());
        out.filterName = filterNameArg.getValue();
        out.lambda = lambdaArg.getValue();
        out.twSize = twSizeArg.getValue();
    } catch( ArgException& e ) {
        LOG(logERROR) << "error: " << e.error() << " for arg " << e.argId();
        return false;
    }

    return true;
}

int main( int argc, char *argv[] )
{
    InputContext ctx;
    if( !parseOptions(argc, argv, ctx) )
        return EXIT_FAILURE;

    mld::SparkseeManager sparkseeManager(ctx.workDir + L"mysparksee.cfg");
    sparkseeManager.openDatabase(ctx.workDir + ctx.dbName + L".sparksee");
    SessionPtr sess(sparkseeManager.newSession());
    sparksee::gdb::Graph* g = sess->GetGraph();

    {
        auto* filter = FilterFactory::create(g, ctx.filterName, ctx.lambda, ctx.twSize);
        TSOperator op(g);
        op.setFilter(filter);

        sess->Begin();
        if( !op.run() ) {
            LOG(logERROR) << "Filtering failed";
            sess->Commit();
            return EXIT_FAILURE;
        }
        sess->Commit();
    }
    LOG(logINFO) << Timer::dumpTrials();
    sess.reset();
    return EXIT_SUCCESS;
}

