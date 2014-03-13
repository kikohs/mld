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

#include <mld/dao/NodeDao.h>

using namespace mld;
using namespace sparksee::gdb;

TEST( NodeDaoTest, CRUD )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<NodeDao> dao( new NodeDao(g) );

    mld::Node a(dao->invalidNode()), b(dao->invalidNode());
    EXPECT_EQ(a, b);

//    mld::Node a2(0, AttrMap());
//    EXPECT_EQ(a, a2);
//    EXPECT_DOUBLE_EQ(kNODE_DEF_VALUE, a2.weight());

    mld::Node n1 = dao->addNode();
    EXPECT_EQ(kNODE_DEF_VALUE, n1.weight());

    n1.setWeight(15);
    n1.setLabel(L"test");
    // Update node content
    EXPECT_TRUE(dao->updateNode(n1));
    // Get
    mld::Node n1_update(dao->getNode(n1.id()));

    EXPECT_EQ(n1.id(), n1_update.id());
    EXPECT_EQ(15, n1_update.weight());
    EXPECT_EQ(L"test", n1_update.label());
    EXPECT_EQ(n1, n1_update);

    // Add user property not persist in db
    n1.data()[L"prop"].SetBooleanVoid(true);
    // Not equal in memory
    EXPECT_NE(n1_update, n1);

    EXPECT_TRUE(dao->updateNode(n1));
    mld::Node n1_bis(dao->getNode(n1.id()));
    // Update the db, drop user prop when read back from db
    EXPECT_EQ(n1_bis, n1_update);

    // GET invalid node
    mld::Node u = dao->getNode(Objects::InvalidOID);
    EXPECT_EQ(Objects::InvalidOID, u.id());

    // Invalid get node, exception is catched if MLD_SAFE flag is defined
#ifdef MLD_SAFE
    mld::Node t = dao->getNode(15434);
    EXPECT_EQ(Objects::InvalidOID, t.id());

    // Invalid remove node, exception is catched if MLD_SAFE flag is defined
    dao->removeNode(123123);
#endif
    // Remove n1 node
    dao->removeNode(n1.id());

#ifdef MLD_SAFE
    u = dao->getNode(n1.id());
    EXPECT_EQ(Objects::InvalidOID, u.id());
#endif

    dao->addNode();
    dao->addNode();

    // Get from Objets
    ObjectsPtr all(g->Select(dao->nodeType()));
    EXPECT_EQ(2, all->Count());
    std::vector<mld::Node> allVec = dao->getNode(all);
    EXPECT_EQ(size_t(2), allVec.size());

    // Add arbitrary prop in nodes
    Value v;
    v.SetDouble(42.0);
    EXPECT_TRUE(sparkseeManager.addAttrToNode(g, L"test", Double, Basic, v));
    // To update model
    dao.reset( new NodeDao(g) );
    auto n2( dao->addNode() );
    EXPECT_DOUBLE_EQ(42.0, n2.data().at(L"test").GetDouble());

    all.reset();
    dao.reset();
    sess.reset();
}
