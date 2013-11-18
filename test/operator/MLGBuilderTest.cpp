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

#include <gtest/gtest.h>

#include <mld/config.h>
#include <mld/DexManager.h>

#include <mld/dao/MLGDao.h>
#include <mld/MLGBuilder.h>
#include <mld/operator/coarseners.h>

using namespace mld;
using namespace dex::gdb;

TEST( MLGBuilderTest, runStepTest )
{

    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );

    Layer base = dao->addBaseLayer();

    // Create nodes
    SuperNode n1 = dao->addNodeToLayer(base);
    SuperNode n2 = dao->addNodeToLayer(base);
    SuperNode n3 = dao->addNodeToLayer(base);
    SuperNode n4 = dao->addNodeToLayer(base);
    SuperNode n5 = dao->addNodeToLayer(base);
    // Node 2 is the heaviest
    n2.setWeight(100);
    dao->updateNode(n2);

    // Create hlinks

    // n4 -- n1 - n2 --- n5
    //       |    /
    //       |   /
    //       |  /
    //       | /
    //       n3
    // Heaviest hlink to collapse
    dao->addHLink(n1, n2, 5);
    dao->addHLink(n1, n4, 4);
    dao->addHLink(n2, n5, 3);
    dao->addHLink(n1, n3);
    dao->addHLink(n2, n3);

    {
        MLGBuilder builder;
        std::shared_ptr<HeavyEdgeCoarsener> coarsener(new HeavyEdgeCoarsener(g));
        // 4 steps of 1 node
        builder.addStep(coarsener);
        builder.addStep(coarsener);
        builder.addStep(coarsener);
        builder.addStep(coarsener);
        bool success = builder.run();
        EXPECT_EQ(success, true);
        Layer top = dao->topLayer();
        EXPECT_EQ(dao->getNodeCount(top), 1);
        EXPECT_EQ(dao->countLayers(), 5);

        // Only 1 node should failed
        builder.addStep(coarsener);
        success = builder.run();
        EXPECT_EQ(success, false);
        // Still 5 layers, mirror should have failed
        EXPECT_EQ(dao->countLayers(), 5);
    }

    dao.reset();
    sess.reset();
}
