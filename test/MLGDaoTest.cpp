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
#include <mld/Graph_types.h>

#include <mld/dao/MLGDao.h>

using namespace mld;
using namespace dex::gdb;


TEST( MLGDaoTest, CRUD )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

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
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

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
//    auto nodeIds = dao->getAllNodeIds(top);
//    EXPECT_EQ(nodeIds->Count(), 3);
//    ObjectsPtr vlinkIds(g->Explode(nodeIds.get(),
//                                 g->FindType(EdgeType::CHILD_OF), dex::gdb::Ingoing));
//    EXPECT_EQ(vlinkIds->Count(), 3);


//    vlinkIds.reset();
//    nodeIds.reset();

    dao.reset();
    sess.reset();
}

