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

#include <mld/dao/LinkDao.h>
#include <mld/dao/SNodeDao.h>

using namespace mld;
using namespace dex::gdb;

TEST( LinkDaoTest, H_LINK_add )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<SNodeDao> snDao( new SNodeDao(g) );

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
    SuperNode n1 = snDao->addNode();
    SuperNode n2 = snDao->addNode();
    SuperNode n3 = snDao->addNode();

    // Add link default weight
    HLink res = lkDao->addHLink(n1.id(), n2.id());
    EXPECT_NE(res.id(), Objects::InvalidOID);
    EXPECT_EQ(res.source(), n1.id());
    EXPECT_EQ(res.target(), n2.id());
    EXPECT_EQ(res.weight(), kHLINK_DEF_VALUE);

    // Add link with weight
    HLink res2 = lkDao->addHLink(n2.id(), n3.id(), 15.0);
    EXPECT_NE(res2.id(), Objects::InvalidOID);
    EXPECT_EQ(res2.source(), n2.id());
    EXPECT_EQ(res2.target(), n3.id());
    EXPECT_EQ(res2.weight(), 15.0);

    // Invalid weight
    HLink res3 = lkDao->addHLink(n3.id(), n1.id(), -415.0);
    EXPECT_EQ(res3.id(), Objects::InvalidOID);

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, V_LINK_add )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<SNodeDao> snDao( new SNodeDao(g) );

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
    SuperNode n1 = snDao->addNode();
    SuperNode n2 = snDao->addNode();
    SuperNode n3 = snDao->addNode();

    // Add link default weight
    VLink res = lkDao->addVLink(n1.id(), n2.id());
    EXPECT_NE(res.id(), Objects::InvalidOID);
    EXPECT_EQ(res.source(), n1.id());
    EXPECT_EQ(res.target(), n2.id());
    EXPECT_EQ(res.weight(), kVLINK_DEF_VALUE);

    // Add link with weight
    VLink res2 = lkDao->addVLink(n2.id(), n3.id(), 15.0);
    EXPECT_NE(res2.id(), Objects::InvalidOID);
    EXPECT_EQ(res2.source(), n2.id());
    EXPECT_EQ(res2.target(), n3.id());
    EXPECT_EQ(res2.weight(), 15.0);

    // Invalid weight
    VLink res3 = lkDao->addVLink(n3.id(), n1.id(), -415.0);
    EXPECT_EQ(res3.id(), Objects::InvalidOID);

    snDao.reset();
    lkDao.reset();
    sess.reset();
}

TEST( LinkDaoTest, H_LINK_get )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<SNodeDao> snDao( new SNodeDao(g) );

#ifdef MLD_SAFE
    // Invalid ids
    auto r = lkDao->getHLink(87687, 4541);
    EXPECT_EQ(r.id(), dex::gdb::Objects::InvalidOID);
    auto r2 = lkDao->getHLink(87687);
    EXPECT_EQ(r2.id(), dex::gdb::Objects::InvalidOID);
#endif

    // Add node first
    SuperNode n1 = snDao->addNode();
    SuperNode n2 = snDao->addNode();
    SuperNode n3 = snDao->addNode();

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
    all.reset(g->Select(snDao->superNodeType()));
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
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<SNodeDao> snDao( new SNodeDao(g) );

#ifdef MLD_SAFE
    // Invalid ids
    auto r = lkDao->getVLink(87687, 4541);
    EXPECT_EQ(r.id(), dex::gdb::Objects::InvalidOID);
    // Invalid id
    auto r2 = lkDao->getVLink(87687);
    EXPECT_EQ(r2.id(), dex::gdb::Objects::InvalidOID);
#endif
    // Add node first
    SuperNode n1 = snDao->addNode();
    SuperNode n2 = snDao->addNode();
    SuperNode n3 = snDao->addNode();

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
    all.reset(g->Select(snDao->superNodeType()));
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
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<SNodeDao> snDao( new SNodeDao(g) );

    bool success = false;
#ifdef MLD_SAFE
    // Remove invalid
    success = lkDao->removeHLink(53541);
    EXPECT_EQ(success, false);
    success = lkDao->removeHLink(53541, 455);
    EXPECT_EQ(success, false);
#endif

    // Add node first
    SuperNode n1 = snDao->addNode();
    SuperNode n2 = snDao->addNode();
    SuperNode n3 = snDao->addNode();

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
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<SNodeDao> snDao( new SNodeDao(g) );

    bool success = false;
#ifdef MLD_SAFE
    // Remove invalid
    success = lkDao->removeVLink(53541);
    EXPECT_EQ(success, false);
    success = lkDao->removeVLink(53541, 455);
    EXPECT_EQ(success, false);
#endif

    // Add node first
    SuperNode n1 = snDao->addNode();
    SuperNode n2 = snDao->addNode();
    SuperNode n3 = snDao->addNode();

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
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<SNodeDao> snDao( new SNodeDao(g) );

    bool success = false;
#ifdef MLD_SAFE
    // Update invalid
    success = lkDao->updateHLink(53541, 45.0);
    EXPECT_EQ(success, false);
    success = lkDao->updateHLink(53541, 455, 78.0);
    EXPECT_EQ(success, false);
#endif

    // Add node first
    SuperNode n1 = snDao->addNode();
    SuperNode n2 = snDao->addNode();
    SuperNode n3 = snDao->addNode();

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
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<LinkDao> lkDao( new LinkDao(g) );
    std::unique_ptr<SNodeDao> snDao( new SNodeDao(g) );

    bool success = false;
#ifdef MLD_SAFE
    // Update invalid
    success = lkDao->updateVLink(53541, 45.0);
    EXPECT_EQ(success, false);
    success = lkDao->updateVLink(53541, 455, 78.0);
    EXPECT_EQ(success, false);
#endif

    // Add node first
    SuperNode n1 = snDao->addNode();
    SuperNode n2 = snDao->addNode();
    SuperNode n3 = snDao->addNode();

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
