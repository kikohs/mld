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

#include <dex/gdb/Graph.h>
#include <dex/gdb/Objects.h>
#include <dex/gdb/ObjectsIterator.h>

#include <mld/common.h>
#include <mld/config.h>
#include <mld/DexManager.h>

#include <mld/operator/mergers.h>
#include <mld/operator/selectors.h>
#include <mld/dao/MLGDao.h>

using namespace mld;
using namespace dex::gdb;

TEST( BasicAdditiveMergerTest, Check1 )
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
    HLink hl12 = dao->addHLink(n1, n2, 5);
    dao->addHLink(n1, n4, 4);
    dao->addHLink(n2, n5, 3);
    HLink hl13 = dao->addHLink(n1, n3);
    HLink hl23 = dao->addHLink(n2, n3);

    // Double mirror to test the merging of parents VLinks
    Layer current = dao->mirrorTopLayer();
    dao->mirrorTopLayer();

    std::unique_ptr<HeavyEdgeSelector> selector( new HeavyEdgeSelector(g) );
    std::unique_ptr<BasicAdditiveMerger> merger( new BasicAdditiveMerger(g) );

    // Do merge
    HLink best = selector->selectBestHLink(current);
    bool success = merger->merge(best, *selector);
    EXPECT_EQ(success, true);
    // Check that a node has actually been merge
    EXPECT_EQ(dao->getNodeCount(base), 5);
    EXPECT_EQ(dao->getNodeCount(current), 4);
    auto hlIds = dao->getAllHLinkIds(current);
    // 5 hlinks at the beginning, there should only be 3 left
    // n1-n3 should have been merge in n2-n3
    // n1-n2 has been collapsed
    EXPECT_EQ(hlIds->Count(), 3);
    hlIds.reset();

    // Check N2 (n2 parent) weight
    auto n2Parents = dao->getParentNodes(n2.id());
    EXPECT_EQ(n2Parents.size(), size_t(1));
    SuperNode N2 = n2Parents[0];
    EXPECT_EQ(N2.weight(), n2.weight() + hl12.weight());
    // Check N2 hlinks, should be 3
    ObjectsPtr N2links(g->Explode(N2.id(), dao->hlinkType(), Outgoing));
    EXPECT_EQ(N2links->Count(), 3);
    N2links.reset();
    // Check that n1 has been merge, VLINK to N2
    auto n1Parents = dao->getParentNodes(n1.id());
    EXPECT_EQ(n1Parents.size(), size_t(1));
    // n1 parent is N2
    EXPECT_EQ(n1Parents[0], N2);

    // Check common edge merging
    // HL23 weight should equal hl23 + hl13 weights
    auto n3Parents = dao->getParentNodes(n3.id());
    EXPECT_EQ(n3Parents.size(), size_t(1));
    HLink HL23 = dao->getHLink(N2.id(), n3Parents[0].id());
    EXPECT_NE(HL23.id(), dex::gdb::Objects::InvalidOID);
    EXPECT_EQ(HL23.weight(), hl13.weight() + hl23.weight());

    // Check N2 parents, should be 2 (1 n1, 1 n2)
    auto N2Parents = dao->getParentNodes(N2.id());
    EXPECT_EQ(N2Parents.size(), size_t(2));

    merger.reset();
    selector.reset();
    dao.reset();
    sess.reset();
}
