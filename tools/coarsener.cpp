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

#include <boost/algorithm/string.hpp>
#include <tclap/CmdLine.h>

#include <locale>
#include <codecvt>
#include <string>

#include <mld/config.h>
#include <mld/SparkseeManager.h>
#include <mld/Session.h>
#include <mld/io/GraphImporter.h>
#include <mld/utils/Timer.h>
#include <mld/MLGBuilder.h>

using namespace TCLAP;
using namespace mld;

struct InputContext {
    std::wstring dbName;
    std::wstring workDir;
    std::string inputPlan;
};

bool parseOptions( int argc, char *argv[], InputContext& out )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    try {
        // Define the command line object.
        CmdLine cmd("Coarsener", ' ', "0.1");

        // Input plan
        ValueArg<std::string> inputArg("s", "steps",
                                       "Coarsening plan\n  \
                                       e.g: H:[0.1,0.2,0.4] X:0.5 \n",
                                       true, "", "string");
        cmd.add(inputArg);
        // Working dir
        ValueArg<std::string> wdArg("d", "workDir", "MLD working directory",
                                    false, converter.to_bytes(mld::kRESOURCES_DIR), "path");
        cmd.add(wdArg);

        // Db Name
        ValueArg<std::string> nameArg("n", "name", "MLD database name (without extension)",
                                    true, "", "string");
        cmd.add(nameArg);

        // Parse the args.
        cmd.parse(argc, argv);

        // Get the value parsed by each arg.
        out.inputPlan = inputArg.getValue();
        out.workDir = converter.from_bytes(wdArg.getValue());
        out.dbName = converter.from_bytes(nameArg.getValue());
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
    {  // MLGBuilder will go out of scope before Session
        MLGBuilder builder;
        if( !builder.fromRawString(g, ctx.inputPlan) ) {
            LOG(logERROR) << "Coarsener: failed to parse input plan";
            sess.reset();
            return EXIT_FAILURE;
        }

        sess->Begin();
        if( !builder.run() ) {
            LOG(logERROR) << "Coarsener: run coarsening plan failed";
        }
        sess->Commit();
    }

    LOG(logINFO) << Timer::dumpTrials();
    sess.reset();
    return EXIT_SUCCESS;
}

