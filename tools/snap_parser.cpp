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

using namespace TCLAP;
using namespace mld;

struct InputContext {
    std::wstring dbName;
    std::wstring workDir;
    std::string inputPath;
};

std::wstring extractDbName( const std::wstring& inputPath )
{
    std::vector<std::wstring> strs;
    boost::algorithm::split(strs, inputPath, boost::is_any_of(L"/"));
    // Get filename from path /test/sdfds.txt
    std::wstring filename = strs.back();
    strs.clear();
    boost::algorithm::split(strs, filename, boost::is_any_of(L"."));
    // Remove last part of .txt for instance
    strs.pop_back();
    return boost::algorithm::join(strs, L".");
}

bool parseOptions( int argc, char *argv[], InputContext& out )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    try {
        // Define the command line object.
        CmdLine cmd("Snap Parser", ' ', "0.1");

        // Define a value argument and add it to the command line.
        ValueArg<std::string> inputArg("i", "input", "undirected snap graph", true, "", "path");
        cmd.add( inputArg );
        ValueArg<std::string> wdArg("d", "workDir", "MLD working directory",
                                    false, converter.to_bytes(mld::kRESOURCES_DIR), "path");
        cmd.add( wdArg );
        // Parse the args.
        cmd.parse( argc, argv );

        // Get the value parsed by each arg.
        out.inputPath = inputArg.getValue();
        out.workDir = converter.from_bytes(wdArg.getValue());
        out.dbName = extractDbName(converter.from_bytes(out.inputPath));
    } catch( ArgException& e ) {
        LOG(logERROR) << "error: " << e.error() << " for arg " << e.argId();
        return false;
    }

    return true;
}

int main( int argc, char *argv[] )
{
    InputContext ctx;
    bool ok = parseOptions(argc, argv, ctx);
    if( !ok )
        return EXIT_FAILURE;

    mld::SparkseeManager sparkseeManager(ctx.workDir + L"mysparksee.cfg");
    sparkseeManager.createDatabase(ctx.workDir + ctx.dbName + L".sparksee", ctx.dbName);

    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);
    sess->Begin();
    GraphImporter::fromSnapFormat(g, ctx.inputPath);
    sess->Commit();
    LOG(logINFO) << Timer::dumpTrials();
    return EXIT_SUCCESS;
}

