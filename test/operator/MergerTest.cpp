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

#include <mld/operator/mergers.h>
#include <mld/dao/MLGDao.h>

using namespace mld;
using namespace sparksee::gdb;

TEST( AdditiveMergerTest, Check )
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
    mld::Node n1 = dao->addNodeToLayer(base);
    n1.setWeight(10);
    dao->updateNode(n1);
    mld::Node n2 = dao->addNodeToLayer(base);
    mld::Node n3 = dao->addNodeToLayer(base);
    mld::Node n4 = dao->addNodeToLayer(base);
    n3.setWeight(20);
    dao->updateNode(n3);

    // Add HLinks
    dao->addHLink(n2, n1, 40);
    dao->addHLink(n2, n3, 2);
    dao->addHLink(n1, n3, 100);
    dao->addHLink(n3, n4, 50);

    // Mirror by hand
    Layer current = dao->addLayerOnTop();
    mld::Node n1c = dao->addNodeToLayer(current);
    mld::Node n2c = dao->addNodeToLayer(current);
    mld::Node n3c = dao->addNodeToLayer(current);
    dao->addVLink(n1, n1c);
    dao->addVLink(n2, n2c);
    dao->addVLink(n2, n1c);
    dao->addVLink(n3, n3c);
    dao->addVLink(n3, n2c);

    dao->addHLink(n2c, n1c, 40);
    dao->addHLink(n2c, n3c, 2);

    // Final M_G

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

    std::unique_ptr<AdditiveNeighborMerger> merger( new AdditiveNeighborMerger(g) );

    ObjectsPtr neighbors(dao->graph()->Neighbors(n2.id(), dao->hlinkType(), Any));
    // n2 = n2 + n1 + n3
    // n2 = 20 + 10 + 1 = 31
    double res = merger->computeWeight(n2, neighbors);
    EXPECT_DOUBLE_EQ(31, res);

    neighbors.reset(dao->graph()->Neighbors(n3.id(), dao->hlinkType(), Any));
    // n3 = n3 + n4 + n2 + n1
    // n3 = 20 + 1 + 1 + 10
    res = merger->computeWeight(n3, neighbors);
    EXPECT_DOUBLE_EQ(32, res);

    // Merge on n3
    neighbors.reset(dao->graph()->Neighbors(n3.id(), dao->hlinkType(), Any));
    bool ok = merger->merge(n3, neighbors);
    EXPECT_TRUE(ok);

    EXPECT_DOUBLE_EQ(32, n3.weight());
    mld::Node newN3 = dao->getNode(n3.id());
    EXPECT_EQ(n3, newN3);

    // Only one node left
    auto count = dao->getNodeCount(base);
    EXPECT_EQ(1, count);
    // Check number of VLinks
    neighbors.reset(dao->graph()->Neighbors(n3.id(), dao->vlinkType(), Any));
    EXPECT_EQ(3, neighbors->Count());

    neighbors.reset();
    merger.reset();
    dao.reset();
    sess.reset();
}
