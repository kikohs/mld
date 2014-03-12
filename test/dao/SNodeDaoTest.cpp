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

TEST( SNodeDaoTest, CRUD )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);

    std::unique_ptr<NodeDao> dao( new NodeDao(g) );

    mld::Node a, b;
    EXPECT_EQ(a, b);

    mld::Node a2(0, AttrMap());
    EXPECT_EQ(a, a2);
    EXPECT_DOUBLE_EQ(kSUPERNODE_DEF_VALUE, a2.weight());

    mld::Node n1 = dao->addNode();
    EXPECT_EQ(n1.weight(), kSUPERNODE_DEF_VALUE);

    n1.setWeight(15);
    n1.setLabel(L"test");
    // Update node content
    dao->updateNode(n1);
    // Get
    mld::Node n1_update = dao->getNode(n1.id());

    EXPECT_EQ(n1.id(), n1_update.id());
    EXPECT_EQ(n1_update.weight(), 15);
    EXPECT_EQ(n1_update.label(), L"test");
    EXPECT_EQ(n1_update, n1);

    // GET invalid node
    mld::Node u = dao->getNode(Objects::InvalidOID);
    EXPECT_EQ(u.id(), Objects::InvalidOID);

    // Invalid get node, exception is catched if MLD_SAFE flag is defined
#ifdef MLD_SAFE
    mld::Node t = dao->getNode(15434);
    EXPECT_EQ(t.id(), Objects::InvalidOID);

    // Invalid remove node, exception is catched if MLD_SAFE flag is defined
    dao->removeNode(123123);
#endif
    // Remove n1 node
    dao->removeNode(n1.id());

#ifdef MLD_SAFE
    u = dao->getNode(n1.id());
    EXPECT_EQ(u.id(), Objects::InvalidOID);
#endif

    dao->addNode();
    dao->addNode();

    // Get from Objets
    ObjectsPtr all(g->Select(dao->superNodeType()));
    EXPECT_EQ(all->Count(), 2);
    std::vector<mld::Node> allVec = dao->getNode(all);
    EXPECT_EQ(allVec.size(), size_t(2));

    all.reset();
    dao.reset();
    sess.reset();
}
