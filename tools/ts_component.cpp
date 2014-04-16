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
#include <mld/io/GraphExporter.h>
#include <mld/utils/Timer.h>
#include <mld/analytics/ComponentExtractor.h>

using namespace TCLAP;
using namespace mld;

struct InputContext {
    std::wstring dbName;
    std::wstring workDir;
    std::string outName;
};

bool parseOptions( int argc, char *argv[], InputContext& out )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    try {
        // Define the command line object.
        CmdLine cmd("TimeSeries component extractor", ' ', "0.1");

        // Working dir
        ValueArg<std::string> wdArg("d", "workDir", "MLD working directory",
                                    false, converter.to_bytes(mld::kRESOURCES_DIR), "path");
        cmd.add(wdArg);

        // Db Name
        ValueArg<std::string> nameArg("n", "dbname", "MLD database name (without extension)",
                                    true, "", "string");
        cmd.add(nameArg);

        // Out name
        ValueArg<std::string> outNameArg("o", "outname", "Output filepath ", true, "", "path");
        cmd.add(outNameArg);

        // Parse the args.
        cmd.parse(argc, argv);

        // Get the value parsed by each arg.
        out.workDir = converter.from_bytes(wdArg.getValue());
        out.dbName = converter.from_bytes(nameArg.getValue());
        out.outName = outNameArg.getValue();
        if( out.outName.empty() ) {
            out.outName = converter.to_bytes(out.dbName);
        }
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

    std::unique_ptr<ComponentExtractor> extractor(new ComponentExtractor(g));

    if( !extractor->run() ) {
        LOG(logERROR) << "Component extraction failed";
        return EXIT_FAILURE;
    }

    if( !GraphExporter::exportVGraphAsJson(extractor->vgraph(), ctx.outName) ) {
        LOG(logERROR) << "Export TS virtual graph failed";
        return EXIT_FAILURE;
    }

    LOG(logINFO) << Timer::dumpTrials();
    extractor.reset();
    sess.reset();
    return EXIT_SUCCESS;
}



