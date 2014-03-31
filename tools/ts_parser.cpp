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
#include <mld/GraphTypes.h>
#include <mld/utils/Timer.h>
#include <mld/operator/TSOperator.h>
#include <mld/operator/filters.h>

using namespace TCLAP;
using namespace mld;

struct InputContext {
    std::wstring dbName;
    std::wstring workDir;
    std::string nodePath;
    std::string edgePath;
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
    // Remove *.nodes or *.edges
    strs.pop_back();
    return boost::algorithm::join(strs, L".");
}

bool parseOptions( int argc, char *argv[], InputContext& out )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    try {
        // Define the command line object.
        CmdLine cmd("TimeSeries graph Parser", ' ', "0.1");

        // Define a value argument and add it to the command line.
        ValueArg<std::string> nodeArg("n", "nodes", "nodes data filepath", true, "", "path");
        cmd.add(nodeArg);
        ValueArg<std::string> edgeArg("e", "edges", "edges data filepath", true, "", "path");
        cmd.add(edgeArg);
        ValueArg<std::string> wdArg("d", "workDir", "MLD working directory",
                                    false, converter.to_bytes(mld::kRESOURCES_DIR), "path");
        cmd.add(wdArg);
        // Parse the args.
        cmd.parse(argc, argv);

        // Get the value parsed by each arg.
        out.nodePath = nodeArg.getValue();
        out.edgePath = edgeArg.getValue();
        out.workDir = converter.from_bytes(wdArg.getValue());
        out.dbName = extractDbName(converter.from_bytes(out.nodePath));
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

    mld::SparkseeManager m(ctx.workDir + L"mysparksee.cfg");
    m.createDatabase(ctx.workDir + ctx.dbName + L".sparksee", ctx.dbName);

    SessionPtr sess(m.newSession());
    sparksee::gdb::Graph* g = sess->GetGraph();
    m.createBaseScheme(g);
    sess->Begin();
    if( !GraphImporter::fromTimeSeries(g, ctx.nodePath, ctx.edgePath) ) {
        LOG(logERROR) << "Error parsing timeseries graph";
        sess->Commit();
        return EXIT_FAILURE;
    }
    sess->Commit();

    TimeVertexMeanFilter* filter = new TimeVertexMeanFilter(g);
    filter->setTimeWindowSize(1);
    TSOperator op(g);
    op.setFilter(filter);

    sess->Begin();
    if( !op.run() ) {
        LOG(logERROR) << "Filtering failed";
        sess->Commit();
        return EXIT_FAILURE;
    }
    sess->Commit();

    LOG(logINFO) << Timer::dumpTrials();
    return EXIT_SUCCESS;
}


