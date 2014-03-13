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
#include <mld/operator/selectors.h>
#include <mld/operator/mergers.h>
#include <mld/dao/MLGDao.h>
#include <mld/utils/Timer.h>

using namespace mld;

TEST( CoarsenerTest, HeavyHLinkCoarsenerTest )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );

    std::unique_ptr<NeighborCoarsener> coarsener( new NeighborCoarsener(g) );
    coarsener->setSelector( new HeavyHLinkSelector(g) );
    coarsener->setMerger( new AdditiveNeighborMerger(g) );
    // Fails
    coarsener->run();

    Layer base = dao->addBaseLayer();

    // Create nodes
    Node n1 = dao->addNodeToLayer(base);
    Node n2 = dao->addNodeToLayer(base);
    Node n3 = dao->addNodeToLayer(base);
    Node n4 = dao->addNodeToLayer(base);
    Node n5 = dao->addNodeToLayer(base);
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
    EXPECT_EQ(int64_t(0), r);

    // Minimum coarsening
    coarsener->setReductionFactor(0.0);
    r = coarsener->computeMergeCount(1000);
    EXPECT_EQ(int64_t(1), r);
    // Maximum coarsening
    coarsener->setReductionFactor(1.0);
    r = coarsener->computeMergeCount(1000);
    EXPECT_EQ(int64_t(999), r);

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
    auto nodes = dao->getAllNodes(top);
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

TEST( CoarsenerTest, XCoarsener )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<NeighborCoarsener> coarsener( new NeighborCoarsener(g) );
    coarsener->setSelector( new XSelector(g) );
    coarsener->setMerger( new AdditiveNeighborMerger(g) );

    Layer base = dao->addBaseLayer();

    // Create nodes
    Node n1 = dao->addNodeToLayer(base);
    Node n2 = dao->addNodeToLayer(base);
    Node n3 = dao->addNodeToLayer(base);
    Node n4 = dao->addNodeToLayer(base);
    Node n5 = dao->addNodeToLayer(base);
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
    Layer top(dao->topLayer());
    auto nodes(dao->getAllNodes(top));

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
