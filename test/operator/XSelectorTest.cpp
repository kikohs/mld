/****************************************************************************
**
** Copyright (C) 2014 EPFL-LTS2
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

#include <mld/config.h>
#include <mld/DexManager.h>

#include <mld/operator/XSelector.h>
#include <mld/dao/MLGDao.h>
#include <mld/utils/Timer.h>

using namespace mld;

// Local variable
typedef std::map<int, dex::gdb::oid_t> NodeMap;
NodeMap nMap;


TEST( XSelectorTest, initScheme )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");

    SessionPtr sess = dexManager.newSession();
    dex::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    dexManager.createScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );

    Layer base = dao->addBaseLayer();

    // Create nodes
    SuperNode n0 = dao->addNodeToLayer(base);
    SuperNode n1 = dao->addNodeToLayer(base);
    SuperNode n2 = dao->addNodeToLayer(base);
    SuperNode n3 = dao->addNodeToLayer(base);
    SuperNode n4 = dao->addNodeToLayer(base);
    // Node 2 is the heaviest
    n1.setWeight(100);
    dao->updateNode(n1);

    // Create hlinks

    // n3 -- n0 - n1 --- n4
    //       |    /
    //       |   /
    //       |  /
    //       | /
    //       n2
    // Heaviest hlink to collapse
    dao->addHLink(n0, n1, 5);
    dao->addHLink(n0, n3, 4);
    dao->addHLink(n1, n4, 3);
    dao->addHLink(n0, n2, 9);
    dao->addHLink(n1, n2);

    nMap[0] = n0.id();
    nMap[1] = n1.id();
    nMap[2] = n2.id();
    nMap[3] = n3.id();
    nMap[4] = n4.id();

    dao.reset();
    sess.reset();
}

TEST( XSelectorTest, Flagging )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");
    SessionPtr sess = dexManager.newSession();
    dex::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );

    {
        ObjectsPtr nodeSet;
        nodeSet = sel->getUnflaggedNeighbors(nMap[3]);
        EXPECT_EQ(1, nodeSet->Count());
        nodeSet = sel->getUnflaggedNeighbors(nMap[0]);
        EXPECT_EQ(3, nodeSet->Count());

        ObjectsPtr nodeSet2 = sel->getUnflaggedNodesFrom(nodeSet);
        EXPECT_TRUE(nodeSet->Equals(nodeSet2.get()));

        // Add a node in flagged set
        sel->flaggedNodes()->Add(nMap[0]);
        EXPECT_TRUE(sel->isFlagged(nMap[0]));

        nodeSet = sel->getUnflaggedNeighbors(nMap[3]);
        EXPECT_EQ(0, nodeSet->Count());

        nodeSet = sel->getUnflaggedNeighbors(nMap[1]);
        EXPECT_EQ(2, nodeSet->Count()); // not n0 because it is flagged

        nodeSet->Add(nMap[0]); // add a flagged node in set
        nodeSet2 = sel->getFlaggedNodesFrom(nodeSet);
        EXPECT_EQ(1, nodeSet2->Count());
    }

    sel.reset();
    dao.reset();
    sess.reset();
}

TEST( XSelectorTest, inOrOutEdges )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");
    SessionPtr sess = dexManager.newSession();
    dex::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );

    // Flag n0
    sel->flaggedNodes()->Add(nMap[0]);
    // Test in-edges retrieval
    {
        ObjectsPtr edgeSet;
        // test in Edges account for flagged nodes
        edgeSet = sel->inOrOutEdges(true, nMap[4], false);
        EXPECT_EQ(1, edgeSet->Count());

        // no flag nodes
        edgeSet = sel->inOrOutEdges(true, nMap[3], false);
        EXPECT_EQ(0, edgeSet->Count());

        // with flagged nodes
        edgeSet = sel->inOrOutEdges(true, nMap[3], true);
        EXPECT_EQ(1, edgeSet->Count());

        // n1 - n4 and n1 - n2, n0 is flagged
        edgeSet = sel->inOrOutEdges(true, nMap[1], false);
        EXPECT_EQ(2, edgeSet->Count());

        // All neighbors, n0 - n2 is in the 1 hop radius
        edgeSet = sel->inOrOutEdges(true, nMap[1], true);
        EXPECT_EQ(4, edgeSet->Count());

        edgeSet = sel->inOrOutEdges(true, nMap[2], false);
        EXPECT_EQ(1, edgeSet->Count());
    }

    // Test out-edges retrieval
    {
        ObjectsPtr edgeSet;
        edgeSet = sel->inOrOutEdges(false, nMap[3], false);
        EXPECT_EQ(0, edgeSet->Count());

        edgeSet = sel->inOrOutEdges(false, nMap[3], true);
        EXPECT_EQ(2, edgeSet->Count());

        // Only n1 - n2, n1 - n0 is flagged
        edgeSet = sel->inOrOutEdges(false, nMap[4], false);
        EXPECT_EQ(1, edgeSet->Count());

        // n1 - n2, n1 - n0
        edgeSet = sel->inOrOutEdges(false, nMap[4], true);
        EXPECT_EQ(2, edgeSet->Count());
    }

    sel.reset();
    dao.reset();
    sess.reset();
}

TEST( XSelectorTest, rootCentralityScore )
{
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.dex", L"MLDTest");
    SessionPtr sess = dexManager.newSession();
    dex::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );

    // Flag n0
    sel->flaggedNodes()->Add(nMap[0]);

    double res = -1.0;
    res = sel->rootCentralityScore(nMap[4], false);
    EXPECT_DOUBLE_EQ(1.0, res);

    // Isolated node because n0 is flagged
    res = sel->rootCentralityScore(nMap[3], false);
    EXPECT_DOUBLE_EQ(0.0, res);

    // Use flagged
    res = sel->rootCentralityScore(nMap[3], true);
    EXPECT_DOUBLE_EQ(1.0, res);

    res = sel->rootCentralityScore(nMap[1], false);
    EXPECT_DOUBLE_EQ(1.0, res);

    // Use flagged
    // traverse edges = 5 + 3 + 1
    // inEdges = traverse edges + n0 - n2 = 18
    res = sel->rootCentralityScore(nMap[1], true);
    EXPECT_DOUBLE_EQ(0.5, res);

    sel->flaggedNodes()->Remove(nMap[0]);
    sel->flaggedNodes()->Add(nMap[4]);

    // do not use flagged nodes
    // traverse edges = 5 + 1
    // inEdges = traverse edges + n0 - n2 = 6 + 9
    res = sel->rootCentralityScore(nMap[1], false);
    double t = 6 / 15.0;
    EXPECT_DOUBLE_EQ(t, res);

    sel.reset();
    dao.reset();
    sess.reset();
}
