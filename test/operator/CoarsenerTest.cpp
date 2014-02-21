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

#include <mld/operator/coarseners.h>
#include <mld/dao/MLGDao.h>
#include <mld/utils/Timer.h>

using namespace mld;

TEST( CoarsenerTest, HeavyEdgeCoarsenerTest )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );

    std::unique_ptr<HeavyEdgeCoarsener> coarsener( new HeavyEdgeCoarsener(g) );
    // Fails
    coarsener->run();

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
    HLink hl12 = dao->addHLink(n1, n2, 5);
    dao->addHLink(n1, n4, 4);
    dao->addHLink(n2, n5, 3);
    HLink hl13 = dao->addHLink(n1, n3);
    HLink hl23 = dao->addHLink(n2, n3);

    // Test merge count
    // Need 2 nodes
    auto r = coarsener->computeMergeCount(0);
    EXPECT_EQ(r, uint64_t(0));

    // Minimum coarsening
    coarsener->setReductionFactor(0.0);
    r = coarsener->computeMergeCount(1000);
    EXPECT_EQ(uint64_t(1), r);
    // Maximum coarsening
    coarsener->setReductionFactor(1.0);
    r = coarsener->computeMergeCount(1000);
    EXPECT_EQ(uint64_t(999), r);

    // 10% reduction over 5 nodes is 0.5, so only 1 node should be merged
    coarsener->setReductionFactor(0.1);
    coarsener->run();

    // Check coarsening result
    Layer top = dao->topLayer();
    EXPECT_EQ(4, dao->getNodeCount(top));

    // Redo coarsening
    coarsener->setReductionFactor(0.99);
    EXPECT_TRUE(coarsener->run());

    // Check supernode
    top = dao->topLayer();
    auto nodes = dao->getAllSuperNode(top);
    EXPECT_EQ(nodes.size(), size_t(1));
    if( !nodes.empty() ) {
        auto node = nodes.at(0);
        EXPECT_EQ(104, node.weight()); // 100 + 1 + 1 + 1 + 1
        auto children = dao->getChildNodes(node.id());
        // Should have 4 children from the previous coarsening pass
        EXPECT_EQ(size_t(4), children.size());
    }

    coarsener.reset();
    dao.reset();
    sess.reset();

//    LOG(logINFO) << Timer::dumpTrials();
}

TEST( CoarsenerTest, XCoarsenerCurrentPassAndMirror )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XCoarsener> coarsener( new XCoarsener(g) );
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

    dao->addHLink(n1, n2, 5);
    dao->addHLink(n1, n4, 4);
    dao->addHLink(n2, n5, 3);
    dao->addHLink(n1, n3);
    dao->addHLink(n2, n3, 7);

    // 10% reduction over 5 nodes is 0.5, so only 1 node should be merged
    // n4 is selected as the best node
    coarsener->setReductionFactor(0.1);
    coarsener->run();

    // 3 nodes should be mirrored
    Layer top = dao->topLayer();
    EXPECT_EQ(4, dao->getNodeCount(top));
    auto n4_new = dao->getNode(n4.id());
    EXPECT_TRUE(n4_new.isRoot());

    auto n4Tops = dao->getParentNodes(n4.id());
    auto n4t = n4Tops.at(0);
    EXPECT_DOUBLE_EQ(2, n4t.weight());
    auto n1Tops = dao->getParentNodes(n1.id());
    auto n1t = n1Tops.at(0);

    // n1 and n4 top nodes are the same
    EXPECT_EQ(n1t, n4t);

    // Get n2 top
    auto n2Tops = dao->getParentNodes(n2.id());
    auto n2t = n2Tops.at(0);
    // Get n3 top
    auto n3Tops = dao->getParentNodes(n3.id());
    auto n3t = n3Tops.at(0);

    // n4 top should be linked to the other tops
    HLink n43t = dao->getHLink(n4t.id(), n3t.id());
    EXPECT_NE(sparksee::gdb::Objects::InvalidOID, n43t.id());
    HLink n42t = dao->getHLink(n4t.id(), n2t.id());
    EXPECT_NE(sparksee::gdb::Objects::InvalidOID, n42t.id());

    // Hlink should exists between n3t and n2t
    HLink n32t = dao->getHLink(n3t.id(), n2t.id());
    EXPECT_NE(sparksee::gdb::Objects::InvalidOID, n32t.id());
    EXPECT_DOUBLE_EQ(7, n32t.weight());

//    // Redo coarsening
//    coarsener->setReductionFactor(1);
//    coarsener->run();

//    // Check supernode
//    top = dao->topLayer();
//    auto nodes = dao->getAllSuperNode(top);
//    EXPECT_EQ(nodes.size(), size_t(1));
//    if( !nodes.empty() ) {
//        auto node = nodes.at(0);
//        EXPECT_EQ(node.weight(), 114); // 100 + 5 + 4 + 3 + 1 + 1
//        auto children = dao->getChildNodes(node.id());
//        // Should have 4 children from the previous coarsening pass
//        EXPECT_EQ(children.size(), size_t(4));
//    }

    coarsener.reset();
    dao.reset();
    sess.reset();

//    LOG(logINFO) << Timer::dumpTrials();
}

TEST( CoarsenerTest, XCoarsenerMultiPassTop )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XCoarsener> coarsener( new XCoarsener(g) );
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

    dao->addHLink(n1, n2, 5);
    dao->addHLink(n1, n4, 4);
    dao->addHLink(n2, n5, 3);
    dao->addHLink(n1, n3);
    dao->addHLink(n2, n3, 7);

    // Full reduction order will be n4, n3 and n5
    coarsener->setReductionFactor(1);
    EXPECT_TRUE(coarsener->run());

    // Check supernode
    Layer top = dao->topLayer();
    auto nodes = dao->getAllSuperNode(top);

    EXPECT_EQ(size_t(1), nodes.size());
    if( !nodes.empty() ) {
        auto node = nodes.at(0);
        EXPECT_DOUBLE_EQ(104, node.weight()); // 100 + 1 + 1 + 1 + 1
        auto children = dao->getChildNodes(node.id());
        // Should have 5 children from the previous coarsening pass
        EXPECT_EQ(size_t(5), children.size());
    }

    // Start another reduction, less than 2 nodes
//    coarsener->setReductionFactor(1);
//    EXPECT_FALSE(coarsener->run());

    coarsener.reset();
    dao.reset();
    sess.reset();

//    LOG(logINFO) << Timer::dumpTrials();
}

TEST( CoarsenerTest, XCoarsenerSinglePass )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XCoarsener> coarsener( new XCoarsener(g) );
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

    dao->addHLink(n1, n2, 5);
    dao->addHLink(n1, n4, 4);
    dao->addHLink(n2, n5, 3);
    dao->addHLink(n1, n3);
    dao->addHLink(n2, n3, 7);

    // Full reduction order will be n4, n3 and n5
    coarsener->setReductionFactor(1);
    coarsener->setSinglePass(true); // will mirror layer
    EXPECT_TRUE(coarsener->run());

    // Check supernode
    Layer top = dao->topLayer();
    auto nodes = dao->getAllSuperNode(top);

    EXPECT_EQ(size_t(1), nodes.size());
    if( !nodes.empty() ) {
        auto node = nodes.at(0);
        EXPECT_DOUBLE_EQ(104, node.weight()); // 100 + 1 + 1 + 1 + 1
        auto children = dao->getChildNodes(node.id());
        // Should have 5 children from the previous coarsening pass
        EXPECT_EQ(size_t(5), children.size());
    }

    coarsener.reset();
    dao.reset();
    sess.reset();

//    LOG(logINFO) << Timer::dumpTrials();
}
