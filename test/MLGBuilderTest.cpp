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
#include <mld/SparkseeManager.h>

#include <mld/dao/MLGDao.h>
#include <mld/MLGBuilder.h>
#include <mld/operator/coarseners.h>
#include <mld/utils/Timer.h>

using namespace mld;
using namespace sparksee::gdb;

TEST( MLGBuilderTest, createFromInputPlan )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);
    std::unique_ptr<MLGBuilder> builder( new MLGBuilder );

    // Test coarsening plan
    std::string inputPlan = "";
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));
    inputPlan = "adf";
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));

    inputPlan = "C:0.2";  // unsupported coarsener
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));

    inputPlan = "H:";
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:[0.1]";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    inputPlan = "hs:[0.1]";
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));

    inputPlan = "X:[0.1]";  // Xs or Xm
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hm:0.1";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:[0.1,0.2]";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:[0.4,0.1]";
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hm:[0.1] X:";
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:[0.1] Xs:[0.1]";
    EXPECT_FALSE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:[0.1] Xs:[0.2]";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:0.1 Xs:[0.2]";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:0.1 Xs:[0.2,0.4]";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:0.1 Xm:0.2";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:0.1 Hm:0.2";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    inputPlan = "Hs:0.1,0.3 Hm:0.4";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    // stil valid
    inputPlan = "Hs:[0.1,0.3 Hm:0.4";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    // stil valid
    inputPlan = "Hs:0.1,0.3] Hm:0.4";
    EXPECT_TRUE(builder->fromRawString(g, inputPlan));

    builder->clearSteps();
}

TEST( MLGBuilderTest, runStepTest )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );

    Layer base = dao->addBaseLayer();

    // Create nodes
    mld::Node n1 = dao->addNodeToLayer(base);
    mld::Node n2 = dao->addNodeToLayer(base);
    mld::Node n3 = dao->addNodeToLayer(base);
    mld::Node n4 = dao->addNodeToLayer(base);
    mld::Node n5 = dao->addNodeToLayer(base);
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


    std::unique_ptr<Timer> t(new Timer("Coarsening benchmark"));
    std::unique_ptr<MLGBuilder> builder( new MLGBuilder );

    CoarsenerPtr coarsener = builder->createCoarsener(g, "Hs", 0.0);
    // 4 steps of 1 node
    builder->addStep(coarsener);
    builder->addStep(coarsener);
    builder->addStep(coarsener);
    builder->addStep(coarsener);
    EXPECT_TRUE(builder->run());
    Layer top = dao->topLayer();
    EXPECT_EQ(1, dao->getNodeCount(top));
    EXPECT_EQ(5, dao->countLayers());

    // Only 1 node should failed
    builder->addStep(coarsener);
    EXPECT_FALSE(builder->run());
    // Still 5 layers, mirror should have failed
    EXPECT_EQ(5, dao->countLayers());
    coarsener.reset();

    builder.reset();
    dao.reset();
    sess.reset();

    LOG(logINFO) << Timer::dumpTrials();
}
