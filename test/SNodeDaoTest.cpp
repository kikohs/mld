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

#include <mld/dao/SNodeDao.h>

using namespace mld;
using namespace dex::gdb;

TEST( SNodeDaoTest, CRUD )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);

    std::unique_ptr<SNodeDao> dao( new SNodeDao(g) );

    SuperNode n1 = dao->addNode();
    EXPECT_EQ(n1.weight(), 0);

    n1.setWeight(15);
    // Update node content
    dao->updateNode(n1);
    // Get
    SuperNode n1_update = dao->getNode(n1.id());

    EXPECT_EQ(n1.id(), n1_update.id());
    EXPECT_EQ(n1_update.weight(), 15);

    // GET invalid node
    SuperNode u = dao->getNode(Objects::InvalidOID);
    EXPECT_EQ(u.id(), Objects::InvalidOID);

    // Invalid get node, exception is catched if MLD_SAFE flag is defined
#ifdef MLD_SAFE
    SuperNode t = dao->getNode(15434);
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

    dao.reset();
    sess.reset();
}
