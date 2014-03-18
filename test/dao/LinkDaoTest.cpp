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

#include <mld/dao/LinkDao.h>
#include <mld/dao/NodeDao.h>

using namespace mld;
using namespace sparksee::gdb;

TEST( LinkDaoTest, H_LINK_add )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

#ifdef MLD_SAFE
    // No nodes in db
    auto src = Objects::InvalidOID;
    // Self loop
    HLink link = lkDao->addHLink(src, src);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    // Invalid src
    link = lkDao->addHLink(src, 54354);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    // self loop
    link = lkDao->addHLink(src, src, 45.0);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    // Invalid src
    link = lkDao->addHLink(src, 54354, 754);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    // Invalid ids
    link = lkDao->addHLink(545, 54354, 754);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    link = lkDao->addHLink(545, 54354);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
#endif

    // Add some nodes to create H_Links
    mld::Node n1 = snDao->addNode();
    mld::Node n2 = snDao->addNode();
    mld::Node n3 = snDao->addNode();

    // Add link default weight
    HLink res = lkDao->addHLink(n1.id(), n2.id());
    EXPECT_NE(res.id(), Objects::InvalidOID);
    EXPECT_EQ(res.source(), n1.id());
    EXPECT_EQ(res.target(), n2.id());
    EXPECT_EQ(res.weight(), HLINK_DEF_VALUE);

    // Add link with weight
    HLink res2 = lkDao->addHLink(n2.id(), n3.id(), 15.0);
    EXPECT_NE(res2.id(), Objects::InvalidOID);
    EXPECT_EQ(res2.source(), n2.id());
    EXPECT_EQ(res2.target(), n3.id());
    EXPECT_EQ(res2.weight(), 15.0);

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, V_LINK_add )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

#ifdef MLD_SAFE
    // No nodes in db
    auto src = Objects::InvalidOID;
    // Self loop
    VLink link = lkDao->addVLink(src, src);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    // Invalid src
    link = lkDao->addVLink(src, 54354);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    link = lkDao->addVLink(src, src, 45.0);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    // Invalid src
    link = lkDao->addVLink(src, 54354, 545);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    // Invalid ids
    link = lkDao->addVLink(753, 54354, 545);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
    link = lkDao->addVLink(753, 54354);
    EXPECT_EQ(link.id(), Objects::InvalidOID);
#endif

    // Add some nodes to create H_Links
    mld::Node n1 = snDao->addNode();
    mld::Node n2 = snDao->addNode();
    mld::Node n3 = snDao->addNode();

    // Add link default weight
    VLink res = lkDao->addVLink(n1.id(), n2.id());
    EXPECT_NE(res.id(), Objects::InvalidOID);
    EXPECT_EQ(res.source(), n1.id());
    EXPECT_EQ(res.target(), n2.id());
    EXPECT_EQ(res.weight(), VLINK_DEF_VALUE);

    // Add link with weight
    VLink res2 = lkDao->addVLink(n2.id(), n3.id(), 15.0);
    EXPECT_NE(res2.id(), Objects::InvalidOID);
    EXPECT_EQ(res2.source(), n2.id());
    EXPECT_EQ(res2.target(), n3.id());
    EXPECT_EQ(res2.weight(), 15.0);

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, H_LINK_get )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

#ifdef MLD_SAFE
    // Invalid ids
    auto r = lkDao->getHLink(87687, 4541);
    EXPECT_EQ(r.id(), sparksee::gdb::Objects::InvalidOID);
    auto r2 = lkDao->getHLink(87687);
    EXPECT_EQ(r2.id(), sparksee::gdb::Objects::InvalidOID);
#endif

    // Add node first
    mld::Node n1 = snDao->addNode();
    mld::Node n2 = snDao->addNode();
    mld::Node n3 = snDao->addNode();

    // Add HLink
    HLink res = lkDao->addHLink(n1.id(), n2.id());

    // Get from DB
    auto res_get = lkDao->getHLink(n1.id(), n2.id());
    EXPECT_EQ(res.id(), res_get.id());
    EXPECT_EQ(res_get.source(), res.source());
    EXPECT_EQ(res_get.target(), res.target());
    EXPECT_EQ(res_get.weight(), res.weight());

    // Test get from link_id
    HLink res2 = lkDao->addHLink(n1.id(), n3.id());
    auto res2_get = lkDao->getHLink(res2.id());
    EXPECT_EQ(res2_get.id(), res2.id());
    EXPECT_EQ(res2_get.source(), res2.source());
    EXPECT_EQ(res2_get.target(), res2.target());
    EXPECT_EQ(res2_get.weight(), res2.weight());

    // Get from Objets
    ObjectsPtr all(g->Select(lkDao->hlinkType()));
    EXPECT_EQ(all->Count(), 2);
    std::vector<HLink> allVec = lkDao->getHLink(all);
    EXPECT_EQ(allVec.size(), size_t(2));

#ifdef MLD_SAFE
    // Get Invalid Objects
    all.reset(g->Select(snDao->nodeType()));
    allVec = lkDao->getHLink(all);
    EXPECT_EQ(allVec.size(), size_t(0));
#endif

    all.reset();
    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, V_LINK_get )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

#ifdef MLD_SAFE
    // Invalid ids
    auto r = lkDao->getVLink(87687, 4541);
    EXPECT_EQ(r.id(), sparksee::gdb::Objects::InvalidOID);
    // Invalid id
    auto r2 = lkDao->getVLink(87687);
    EXPECT_EQ(r2.id(), sparksee::gdb::Objects::InvalidOID);
#endif
    // Add node first
    mld::Node n1 = snDao->addNode();
    mld::Node n2 = snDao->addNode();
    mld::Node n3 = snDao->addNode();

    // Add VLink
    VLink res = lkDao->addVLink(n1.id(), n2.id());
    EXPECT_NE(res.id(), Objects::InvalidOID);

    // Get from DB
    auto res_get = lkDao->getVLink(n1.id(), n2.id());
    EXPECT_EQ(res.id(), res_get.id());
    EXPECT_EQ(res_get.source(), res.source());
    EXPECT_EQ(res_get.target(), res.target());
    EXPECT_EQ(res_get.weight(), res.weight());

    // Test get from link_id
    VLink res2 = lkDao->addVLink(n1.id(), n3.id());
    auto res2_get = lkDao->getVLink(res2.id());
    EXPECT_EQ(res2_get.id(), res2.id());
    EXPECT_EQ(res2_get.source(), res2.source());
    EXPECT_EQ(res2_get.target(), res2.target());
    EXPECT_EQ(res2_get.weight(), res2.weight());

    // Get from Objets
    ObjectsPtr all(g->Select(lkDao->vlinkType()));
    EXPECT_EQ(all->Count(), 2);
    std::vector<VLink> allVec = lkDao->getVLink(all);
    EXPECT_EQ(allVec.size(), size_t(2));

#ifdef MLD_SAFE
    // Get Invalid Objects
    all.reset(g->Select(snDao->nodeType()));
    allVec = lkDao->getVLink(all);
    EXPECT_EQ(allVec.size(), size_t(0));
#endif

    all.reset();
    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, H_LINK_remove )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

    bool success = false;
#ifdef MLD_SAFE
    // Remove invalid
    success = lkDao->removeHLink(53541);
    EXPECT_EQ(success, false);
    success = lkDao->removeHLink(53541, 455);
    EXPECT_EQ(success, false);
#endif

    // Add node first
    mld::Node n1 = snDao->addNode();
    mld::Node n2 = snDao->addNode();
    mld::Node n3 = snDao->addNode();

    // Add HLinks
    auto hl1 = lkDao->addHLink(n1.id(), n2.id());
    auto hl2 = lkDao->addHLink(n1.id(), n3.id());

    // Remove first link
    success = lkDao->removeHLink(hl1.source(), hl1.target());
    EXPECT_EQ(success, true);

#ifdef MLD_SAFE
    // Remove again, should return false
    success = lkDao->removeHLink(hl1.source(), hl1.target());
    EXPECT_EQ(success, false);
#endif

    // Remove by link id
    success = lkDao->removeHLink(hl2.id());
    EXPECT_EQ(success, true);

#ifdef MLD_SAFE
    // Remove again, should return false
    success = lkDao->removeHLink(hl2.id());
    EXPECT_EQ(success, false);
#endif

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, V_LINK_remove )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

    bool success = false;
#ifdef MLD_SAFE
    // Remove invalid
    success = lkDao->removeVLink(53541);
    EXPECT_EQ(success, false);
    success = lkDao->removeVLink(53541, 455);
    EXPECT_EQ(success, false);
#endif

    // Add node first
    mld::Node n1 = snDao->addNode();
    mld::Node n2 = snDao->addNode();
    mld::Node n3 = snDao->addNode();

    // Add VLinks
    auto hl1 = lkDao->addVLink(n1.id(), n2.id());
    auto hl2 = lkDao->addVLink(n1.id(), n3.id());

    // Remove first link
    success = lkDao->removeVLink(hl1.source(), hl1.target());
    EXPECT_EQ(success, true);

#ifdef MLD_SAFE
    // Remove again, should return false
    success = lkDao->removeVLink(hl1.source(), hl1.target());
    EXPECT_EQ(success, false);
#endif

    // Remove by link id
    success = lkDao->removeVLink(hl2.id());
    EXPECT_EQ(success, true);

#ifdef MLD_SAFE
    // Remove again, should return false
    success = lkDao->removeVLink(hl2.id());
    EXPECT_EQ(success, false);
#endif

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, H_LINK_update )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

    bool success = false;
#ifdef MLD_SAFE
    // Update invalid
    success = lkDao->updateHLink(53541, 45.0);
    EXPECT_EQ(success, false);
    success = lkDao->updateHLink(53541, 455, 78.0);
    EXPECT_EQ(success, false);
#endif

    // Add node first
    mld::Node n1 = snDao->addNode();
    mld::Node n2 = snDao->addNode();
    mld::Node n3 = snDao->addNode();

    // Add HLinks
    auto hl1 = lkDao->addHLink(n1.id(), n2.id());
    auto hl2 = lkDao->addHLink(n1.id(), n3.id());

    // Update weight
    hl1.setWeight(10.0);
    success = lkDao->updateHLink(hl1.source(), hl1.target(), hl1.weight());
    EXPECT_EQ(success, true);
    // Get form db to check
    auto hl1_get = lkDao->getHLink(hl1.id());
    EXPECT_EQ(hl1_get.weight(), hl1.weight());

    // Update weight
    hl2.setWeight(50.0);
    success = lkDao->updateHLink(hl2.id(), hl2.weight());
    EXPECT_EQ(success, true);
    // Get form db to check
    auto hl2_get = lkDao->getHLink(hl2.source(), hl2.target());
    EXPECT_EQ(hl2_get.weight(), hl2.weight());

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, V_LINK_update )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

    bool success = false;
#ifdef MLD_SAFE
    // Update invalid
    success = lkDao->updateVLink(53541, 45.0);
    EXPECT_EQ(success, false);
    success = lkDao->updateVLink(53541, 455, 78.0);
    EXPECT_EQ(success, false);
#endif

    // Add node first
    mld::Node n1 = snDao->addNode();
    mld::Node n2 = snDao->addNode();
    mld::Node n3 = snDao->addNode();

    // Add VLinks
    auto vl1 = lkDao->addVLink(n1.id(), n2.id());
    auto vl2 = lkDao->addVLink(n1.id(), n3.id());

    // Update weight
    vl1.setWeight(10.0);
    success = lkDao->updateVLink(vl1.source(), vl1.target(), vl1.weight());
    EXPECT_EQ(success, true);
    // Get form db to check
    auto vl1_get = lkDao->getVLink(vl1.id());
    EXPECT_EQ(vl1_get.weight(), vl1.weight());

    // Update weight
    vl2.setWeight(50.0);
    success = lkDao->updateVLink(vl2.id(), vl2.weight());
    EXPECT_EQ(success, true);
    // Get form db to check
    auto vl2_get = lkDao->getVLink(vl2.source(), vl2.target());
    EXPECT_EQ(vl2_get.weight(), vl2.weight());

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, ExtraProp )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);
    Value v;
    v.SetDouble(42.0);
    EXPECT_TRUE(sparkseeManager.addAttrToHLink(g, L"Htest", Double, Basic, v));
    EXPECT_TRUE(sparkseeManager.addAttrToVLink(g, L"Vtest", Double, Basic, v));

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

    mld::Node n1(snDao->addNode());
    mld::Node n2(snDao->addNode());

    HLink hl(lkDao->addHLink(n1.id(), n2.id()));
    EXPECT_DOUBLE_EQ(42.0, hl.data()[L"Htest"].GetDouble());
    VLink vl(lkDao->addVLink(n1.id(), n2.id()));
    EXPECT_DOUBLE_EQ(42.0, vl.data()[L"Vtest"].GetDouble());

    hl.data()[L"Htest"].SetDoubleVoid(50);
    EXPECT_TRUE(lkDao->updateHLink(hl.source(), hl.target(), hl.data()));
    HLink hl2(lkDao->getHLink(hl.id()));
    EXPECT_EQ(hl2, hl);

    vl.data()[L"Vtest"].SetDoubleVoid(50);
    EXPECT_TRUE(lkDao->updateVLink(vl.source(), vl.target(), vl.data()));
    VLink vl2(lkDao->getVLink(vl.id()));
    EXPECT_EQ(vl2, vl);

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, OLinkCRUD )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<NodeDao> snDao( new NodeDao(g) );

    oid_t layerType = g->FindType(NodeType::LAYER);
    oid_t lid = g->NewNode(layerType);

    bool success = false;
#ifdef MLD_SAFE
    // Update invalid
    success = lkDao->updateOLink(53541, 45.0);
    EXPECT_FALSE(success);
    success = lkDao->updateVLink(53541, 455, 78.0);
    EXPECT_FALSE(success);
#endif

    // Add node first
    mld::Node n1 = snDao->addNode();
    OLink ol1 = lkDao->addOLink(lid, n1.id());
    EXPECT_NE(Objects::InvalidOID, ol1.id());

    mld::Node n2 = snDao->addNode();
    OLink ol2 = lkDao->addOLink(lid, n2.id(), 12.0);
    EXPECT_NE(Objects::InvalidOID, ol2.id());
    EXPECT_DOUBLE_EQ(12.0, ol2.weight());

    ol1.setWeight(12.0);
    success = lkDao->updateOLink(ol1.id(), ol1.data());
    EXPECT_TRUE(success);
    ol1 = lkDao->getOLink(ol1.id());
    EXPECT_DOUBLE_EQ(ol1.weight(), ol2.weight());
}
