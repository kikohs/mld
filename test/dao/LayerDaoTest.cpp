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

#include <mld/dao/LayerDao.h>

using namespace mld;
using namespace sparksee::gdb;

//TEST( LayerDaoTest, init )
//{
//    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
//    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

//    SessionPtr sess = sparkseeManager.newSession();
//    Graph* g = sess->GetGraph();
//    // Create Db scheme
//    sparkseeManager.createScheme(g);
//    sess.reset();
//}

TEST( LayerDaoTest, BaseLayer )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);

    std::unique_ptr<LayerDao> dao( new LayerDao(g) );

    // No layers yet
    int64_t nb = dao->countLayers();
    EXPECT_EQ(nb, 0);
    // No base layer is defined
    Layer res = dao->addLayerOnTop();
    EXPECT_EQ(res.id(), Objects::InvalidOID);
    // No base layer is defined
    Layer res2 = dao->addLayerOnBottom();
    EXPECT_EQ(res2.id(), Objects::InvalidOID);

    // Create base layer
    Layer base = dao->addBaseLayer();
    EXPECT_NE(base.id(), Objects::InvalidOID);
    EXPECT_EQ(base.isBaseLayer(), true);
    // Try to recreate a base layer
    Layer base2 = dao->addBaseLayer();
    EXPECT_EQ(base2.id(), Objects::InvalidOID);

    // No parent yet
    Layer parent = dao->parent(base);
    EXPECT_EQ(parent.id(), Objects::InvalidOID);
    // No child
    Layer child = dao->child(base);
    EXPECT_EQ(child.id(), Objects::InvalidOID);

    // Only 1 layer, top == base
    Layer top = dao->topLayer();
    EXPECT_EQ(top.id(), base.id());

    // Only 1 layer, bottom == base
    Layer bottom = dao->topLayer();
    EXPECT_EQ(bottom.id(), base.id());

    // Only baseLayer
    EXPECT_EQ(dao->countLayers(), 1);

    dao.reset();
    sess.reset();
}

TEST( LayerDaoTest, AddRemoveLayers )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);

    std::unique_ptr<LayerDao> dao( new LayerDao(g) );
    bool success = false;
    // No layers yet
    EXPECT_EQ(0, dao->countLayers());
    // Create base layer
    Layer base = dao->addBaseLayer();
    EXPECT_NE(base.id(), Objects::InvalidOID);
    EXPECT_NE(base.id(), 0);
    EXPECT_EQ(base.isBaseLayer(), true);

    // Add Layers on top and bottom
    Layer top = dao->addLayerOnTop();
    EXPECT_NE(top.id(), Objects::InvalidOID);
    Layer bot = dao->addLayerOnBottom();
    EXPECT_NE(bot.id(), Objects::InvalidOID);

    EXPECT_EQ(3, dao->countLayers());

    // Set top layer as new base
    dao->setAsBaseLayer(top);

    // Check that child is the previous base layer
    Layer exbase = dao->child(top);
    EXPECT_EQ(exbase.id(), base.id());

    // Check bottom layer
    Layer bot2 = dao->child(exbase);
    EXPECT_EQ(bot2.id(), bot.id());

    // Check affiliation
    success = dao->affiliated(exbase.id(), bot2.id());
    EXPECT_EQ(success, true);
    success = dao->affiliated(exbase.id(), top.id());
    EXPECT_EQ(success, true);
    success = dao->affiliated(bot2.id(), top.id());
    EXPECT_EQ(success, false);

    // Remove top layer, should not work, toplayer is base
    success = dao->removeTopLayer();
    EXPECT_EQ(success, false);

    EXPECT_EQ(3, dao->countLayers());

    // Remove bottom layer
    success = dao->removeBottomLayer();
    EXPECT_EQ(success, true);
    EXPECT_EQ(dao->countLayers(), 2);

    // Set reset base layer
    dao->setAsBaseLayer(base);

    // Remove toplayer
    success = dao->removeTopLayer();
    EXPECT_EQ(success, true);
    EXPECT_EQ(dao->countLayers(), 1);

    // Test remove base layer
    success = dao->removeBaseLayer();
    EXPECT_EQ(success, true);
    EXPECT_EQ(dao->countLayers(), 0);

    // Test remove
    base = dao->addBaseLayer();
    dao->addLayerOnTop();
    dao->addLayerOnTop();
    dao->addLayerOnTop();
    dao->addLayerOnBottom();
    dao->addLayerOnBottom();
    dao->addLayerOnBottom();
    auto last = dao->addLayerOnBottom();

    success = dao->removeBaseLayer();
    EXPECT_EQ(success, false);
    EXPECT_EQ(dao->countLayers(), 8);

    success = dao->removeAllButBaseLayer();
    EXPECT_EQ(success, true);
    EXPECT_EQ(dao->countLayers(), 1);
    EXPECT_EQ(base.id(), dao->baseLayer().id());

    // Check if exists
    success = dao->exists(base);
    EXPECT_EQ(success, true);

    // All should have been removed
    success = dao->exists(last);
    EXPECT_EQ(success, false);

    dao.reset();
    sess.reset();
}

TEST( LayerDaoTest, GetUpdateLayer )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);

    std::unique_ptr<LayerDao> dao( new LayerDao(g) );

    auto base = dao->addBaseLayer();
    base.setDescription(L"Base layer blabla");
    dao->updateLayer(base);

    auto base_get = dao->getLayer(base.id());
    EXPECT_EQ(base_get.id(), base.id());
    EXPECT_EQ(base_get.description(), base.description());
    EXPECT_EQ(base_get.isBaseLayer(), true);

    dao.reset();
    sess.reset();
}
