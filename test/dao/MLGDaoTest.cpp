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

#include <sparksee/gdb/Graph.h>
#include <sparksee/gdb/Objects.h>
#include <sparksee/gdb/ObjectsIterator.h>

#include <mld/common.h>
#include <mld/config.h>
#include <mld/SparkseeManager.h>
#include <mld/Graph_types.h>

#include <mld/dao/MLGDao.h>

using namespace mld;
using namespace sparksee::gdb;


TEST( MLGDaoTest, CRUD )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);

    std::unique_ptr<MLGDao> dao( new MLGDao(g) );

    bool success = false;
    Layer base = dao->addBaseLayer();

    SuperNode n1 = dao->addNodeToLayer(base);
    SuperNode n2 = dao->addNodeToLayer(base);
    // Check node ownership
    ObjectsPtr neighbors(g->Neighbors(base.id(), g->FindType(EdgeType::OWNS), Outgoing));
    success = neighbors->Exists(n1.id());
    EXPECT_EQ(success, true);
    neighbors.reset();

    // Check that the nodes belongs to the same layer
    HLink hl12 = dao->addHLink(n1, n2);
    EXPECT_EQ(hl12.source(), n1.id());
    EXPECT_EQ(hl12.target(), n2.id());
    EXPECT_NE(hl12.id(), Objects::InvalidOID);

    // Add a top layer
    Layer top = dao->addLayerOnTop();
    SuperNode n3 = dao->addNodeToLayer(top);
    // Check node count
    EXPECT_EQ(dao->getNodeCount(top), 1);

    // Not on the same layer
    HLink hl13 = dao->addHLink(n1, n3);
    EXPECT_EQ(hl13.id(), Objects::InvalidOID);

    // Check that node are on consecutive layers
    VLink vl13 = dao->addVLink(n1, n3);
    EXPECT_EQ(vl13.source(), n1.id());
    EXPECT_EQ(vl13.target(), n3.id());
    EXPECT_NE(vl13.id(), Objects::InvalidOID);

    // Node are on the same layer, invalid
    VLink vl12 = dao->addVLink(n1, n2);
    EXPECT_EQ(vl12.id(), Objects::InvalidOID);

    Layer bot = dao->addLayerOnBottom();
    SuperNode n4 = dao->addNodeToLayer(bot);

    // Node are not a consecutive layer, invalid
    VLink vl34 = dao->addVLink(n3, n4);
    EXPECT_EQ(vl34.id(), Objects::InvalidOID);

    // Get Node for each layer
    // Base layer
    ObjectsPtr baseNodes = dao->getAllNodeIds(base);
    EXPECT_EQ(baseNodes->Count(), 2);
    baseNodes.reset();
    auto nodes = dao->getAllSuperNode(base);
    EXPECT_EQ(nodes.size(), size_t(2));

    // Top layer
    ObjectsPtr topNodes = dao->getAllNodeIds(top);
    EXPECT_EQ(topNodes->Count(), 1);
    topNodes.reset();
    nodes = dao->getAllSuperNode(top);
    EXPECT_EQ(nodes.size(), size_t(1));
    EXPECT_EQ(nodes.at(0), n3);

    // New bottom layer no nodes
    bot = dao->addLayerOnBottom();
    ObjectsPtr noNodes = dao->getAllNodeIds(bot);
    EXPECT_EQ(noNodes->Count(), 0);
    EXPECT_EQ(dao->getNodeCount(bot), 0);
    noNodes.reset();
    nodes = dao->getAllSuperNode(bot);
    EXPECT_EQ(nodes.empty(), true);

    // Get HLink for base layer
    auto hlinkIds = dao->getAllHLinkIds(base);
    EXPECT_EQ(hlinkIds->Count(), 1);
    hlinkIds.reset();

    // Should return HLink n1 - n2
    auto hlinks = dao->getAllHLinks(base);
    EXPECT_EQ(hlinks.at(0).id(), hl12.id());

    // Empty layer
    auto noHlinks = dao->getAllHLinkIds(bot);
    EXPECT_EQ(noHlinks->Count(), 0);
    noHlinks.reset();
    hlinks = dao->getAllHLinks(bot);
    EXPECT_EQ(hlinks.empty(), true);

    dao.reset();
    sess.reset();
}

TEST( MLGDaoTest, MirrorLayer )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);

    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    Layer base = dao->addBaseLayer();

    // Add nodes
    SuperNode n1 = dao->addNodeToLayer(base);
    n1.setWeight(10);
    dao->updateNode(n1);
    SuperNode n2 = dao->addNodeToLayer(base);
    SuperNode n3 = dao->addNodeToLayer(base);
    n3.setWeight(20);
    dao->updateNode(n3);

    // Add HLinks
    HLink hl21 = dao->addHLink(n2, n1, 40);
    HLink hl23 = dao->addHLink(n2, n3, 2);

    // Mirror Layers
    Layer top = dao->mirrorTopLayer();
    EXPECT_NE(top.id(), Objects::InvalidOID);

    auto nodes = dao->getAllSuperNode(top);
    EXPECT_EQ(nodes.size(), size_t(3));

    auto hlinks = dao->getAllHLinks(top);
    EXPECT_EQ(hlinks.size(), size_t(2));

    // Check parent node
    auto p = dao->getParentNodes(n1.id());
    EXPECT_EQ(p.size(), size_t(1));
    SuperNode parentN1;
    if( !p.empty() )
        parentN1 = p.at(0);
    EXPECT_EQ(parentN1.weight(), n1.weight());

    // Check child node
    SuperNode n1_bis;
    auto n1_bisvec = dao->getChildNodes(parentN1.id());
    EXPECT_EQ(n1_bisvec.empty(), false);
    if( !n1_bisvec.empty() )
        n1_bis = n1_bisvec.at(0);
    EXPECT_EQ(n1, n1_bis);

    ObjectsPtr ed(g->Explode(parentN1.id(), g->FindType(EdgeType::H_LINK), Outgoing));
    EXPECT_EQ(ed->Count(), 1);
    HLink pHl21 = dao->getHLink(ed->Any());
    ed.reset();
    EXPECT_EQ(pHl21.weight(), hl21.weight());

    dao.reset();
    sess.reset();
}

TEST( MLGDaoTest, GetHeaviestHLink )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);

    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    Layer base = dao->addBaseLayer();

    // Add nodes
    SuperNode n1 = dao->addNodeToLayer(base);
    n1.setWeight(10);
    dao->updateNode(n1);
    SuperNode n2 = dao->addNodeToLayer(base);
    SuperNode n3 = dao->addNodeToLayer(base);
    n3.setWeight(20);
    dao->updateNode(n3);

    // Add HLinks
    HLink hl21 = dao->addHLink(n2, n1, 40);
    dao->addHLink(n2, n3, 2);

    // Mirror Layers
    Layer top = dao->mirrorTopLayer();

    // Get heaviest edge, use trick
    HLink maxHlink = dao->getUnsafeHeaviestHLink();
    EXPECT_GT(maxHlink.id(), hl21.id());
    EXPECT_FLOAT_EQ(maxHlink.weight(), 40);

    // Add new hlink in base layer not top layer
    dao->addHLink(n1, n3, 10);
    // Trick should work because, the new hlink has not the max value
    auto maxHlink2 = dao->getUnsafeHeaviestHLink();
    EXPECT_EQ(maxHlink.id(), maxHlink2.id());
    EXPECT_FLOAT_EQ(maxHlink.weight(), maxHlink2.weight());

    // Add new hlink in base layer with the highest value
    SuperNode n4 = dao->addNodeToLayer(base);
    HLink hl41 = dao->addHLink(n4, n1, 50);

    // It should fail, the return maxlink is not in the top layer
    auto maxHlink3 = dao->getUnsafeHeaviestHLink();
    // The layer is not checked
    EXPECT_EQ(maxHlink3.id(), hl41.id());
    EXPECT_FLOAT_EQ(maxHlink3.weight(), hl41.weight());

    // DISABLE TRICK, layer should account
    auto maxHlink4 = dao->getHeaviestHLink(top);
    // The layer should be checked
    EXPECT_EQ(maxHlink4.id(), maxHlink.id());
    EXPECT_FLOAT_EQ(maxHlink4.weight(), maxHlink.weight());

    dao.reset();
    sess.reset();
}

TEST( MLGDaoTest, copyAndMergeLinks )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);

    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    Layer base = dao->addBaseLayer();

    // Add nodes
    SuperNode n1 = dao->addNodeToLayer(base);
    n1.setWeight(10);
    dao->updateNode(n1);
    SuperNode n2 = dao->addNodeToLayer(base);
    SuperNode n3 = dao->addNodeToLayer(base);
    SuperNode n4 = dao->addNodeToLayer(base);
    n3.setWeight(20);
    dao->updateNode(n3);

    // Add HLinks
    dao->addHLink(n2, n1, 40);
    dao->addHLink(n2, n3, 2);
    dao->addHLink(n1, n3, 100);
    dao->addHLink(n3, n4, 50);

    // Mirror by hand
    Layer current = dao->addLayerOnTop();
    SuperNode n1c = dao->addNodeToLayer(current);
    SuperNode n2c = dao->addNodeToLayer(current);
    SuperNode n3c = dao->addNodeToLayer(current);
    dao->addVLink(n1, n1c);
    dao->addVLink(n2, n2c);
    dao->addVLink(n2, n1c);
    dao->addVLink(n3, n3c);
    dao->addVLink(n3, n2c);

    dao->addHLink(n2c, n1c, 40);
    dao->addHLink(n2c, n3c, 2);

    // Mirror again, we have 3 layers
    Layer top = dao->addLayerOnTop();
    SuperNode n1t = dao->addNodeToLayer(top);
    SuperNode n2t = dao->addNodeToLayer(top);
    SuperNode n3t = dao->addNodeToLayer(top);
    dao->addVLink(n1c, n1t);
    dao->addVLink(n2c, n2t);
    dao->addVLink(n3c, n3t);
    dao->addVLink(n2c, n1t);

    dao->addHLink(n2t, n1t, 40);
    dao->addHLink(n2t, n3t, 2);

    // Final M_G

    //  n1t ------- n2t ------- n3t
    //   |  \       |           |
    //   |   \      |           |
    //   |    \     |           |
    //   |     \    |           |
    //   |      \   |           |
    //   |       \  |           |
    //   |        \ |           |
    //  n1c ------- n2c ------- n3c
    //   | \        |  \        |
    //   |  \       |   \       |
    //   |   \      |    \      |
    //   |    \     |     \     |
    //   |     \    |      \    |
    //   |      \   |       \   |
    //   |       \  |        \  |
    //   |        \ |         \ |
    //  n1 -------- n2 -------- n3 ---- n4
    //   \______________________/

    // Test HLinks, n3 - n1 should be added to n1 - n2 and
    // n2 - n4 should have been created

    bool ok = dao->copyAndMergeHLinks(n3, n2);
    EXPECT_TRUE(ok);
    HLink h12 = dao->getHLink(n1.id(), n2.id());
    EXPECT_DOUBLE_EQ(140, h12.weight());
    HLink h24 = dao->getHLink(n2.id(), n4.id());
    EXPECT_NE(Objects::InvalidOID, h24.id());

    // Test VLINKs

    // n3 - n2c should be added to n2 - n2c
    // n2 - n3c should be created
    ok = dao->copyAndMergeVLinks(n3, n2);
    EXPECT_TRUE(ok);
    VLink v2_2c = dao->getVLink(n2.id(), n2c.id());
    EXPECT_DOUBLE_EQ(2, v2_2c.weight());
    VLink v2_3c = dao->getVLink(n2.id(), n3c.id());
    EXPECT_NE(Objects::InvalidOID, v2_3c.id());

    // n1c - n1t should be added to n2c - n1t
    // n2 - n1c should be added n2 - n2c
    // n1 - n2c should be created
    ok = dao->copyAndMergeVLinks(n1c, n2c);
    EXPECT_TRUE(ok);
    VLink v2c_1t = dao->getVLink(n2c.id(), n1t.id());
    EXPECT_DOUBLE_EQ(2, v2c_1t.weight());
    v2_2c = dao->getVLink(n2.id(), n2c.id());
    EXPECT_DOUBLE_EQ(3, v2_2c.weight()); // 1 from previous merge
    VLink v1_2c = dao->getVLink(n1.id(), n2c.id());
    EXPECT_NE(Objects::InvalidOID, v1_2c.id());

    // n1c - n3t and n2c - n3t should be created
    ok = dao->copyAndMergeVLinks(n1t, n3t);
    EXPECT_TRUE(ok);
    VLink v1c_3t = dao->getVLink(n1c.id(), n3t.id());
    EXPECT_NE(Objects::InvalidOID, v1c_3t.id());
    VLink v2c_3t = dao->getVLink(n1.id(), n2c.id());
    EXPECT_NE(Objects::InvalidOID, v2c_3t.id());

    // Test merge nodes not on same layer
    ok = dao->copyAndMergeVLinks(n1, n1c);
    EXPECT_FALSE(ok);

    dao.reset();
    sess.reset();
}
