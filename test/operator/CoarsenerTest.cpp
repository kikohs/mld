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

#include <mld/operator/coarseners.h>
#include <mld/dao/MLGDao.h>
#include <mld/utils/Timer.h>

using namespace mld;

TEST( CoarsenerTest, HeavyEdgeCoarsenerTest )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    dex::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);
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
    EXPECT_EQ(r, uint64_t(1));
    // Maximum coarsening
    coarsener->setReductionFactor(1.0);
    r = coarsener->computeMergeCount(1000);
    EXPECT_EQ(r, uint64_t(999));

    // 10% reduction over 5 nodes is 0.5, so only 1 node should be merged
    coarsener->setReductionFactor(0.1);
    coarsener->run();

    // Check coarsening result
    Layer top = dao->topLayer();
    EXPECT_EQ(dao->getNodeCount(top), 4);

    // Redo coarsening
    coarsener->setReductionFactor(1);
    coarsener->run();

    // Check supernode
    top = dao->topLayer();
    auto nodes = dao->getAllSuperNode(top);
    EXPECT_EQ(nodes.size(), size_t(1));
    if( !nodes.empty() ) {
        auto node = nodes.at(0);
        EXPECT_EQ(node.weight(), 114); // 100 + 5 + 4 + 3 + 1 + 1
        auto children = dao->getChildNodes(node.id());
        // Should have 4 children from the previous coarsening pass
        EXPECT_EQ(children.size(), size_t(4));
    }

    coarsener.reset();
    dao.reset();
    sess.reset();

    LOG(logINFO) << Timer::dumpTrials();
}
