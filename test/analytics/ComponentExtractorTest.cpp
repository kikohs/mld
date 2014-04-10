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

#include <gtest/gtest.h>

#include <mld/config.h>
#include <mld/SparkseeManager.h>

#include <mld/dao/MLGDao.h>
#include <mld/analytics/ComponentExtractor.h>

using namespace mld;
using namespace sparksee::gdb;

TEST( ComponentExtractorTest, ExtractComponentGraph )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    // Use shared instead of unique to have QtCreator linter ...
    std::shared_ptr<ComponentExtractor> extract(new ComponentExtractor(g));
    extract->run();

    extract.reset();
    sess.reset();
}
