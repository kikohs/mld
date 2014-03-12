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

#include <sparksee/gdb/Objects.h>
#include <mld/config.h>
#include <mld/SparkseeManager.h>

#include <mld/operator/selector/XSelector.h>
#include <mld/dao/MLGDao.h>
#include <mld/utils/Timer.h>

using namespace mld;

// Local variable
typedef std::map<int, sparksee::gdb::oid_t> NodeMap;
NodeMap nMap;

TEST( XSelectorTest, initScheme )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createScheme(g);
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );

    Layer base = dao->addBaseLayer();

    // Create nodes
    Node n0 = dao->addNodeToLayer(base);
    Node n1 = dao->addNodeToLayer(base);
    Node n2 = dao->addNodeToLayer(base);
    Node n3 = dao->addNodeToLayer(base);
    Node n4 = dao->addNodeToLayer(base);
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

TEST( XSelectorTest, inOrOutEdges )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");
    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );

    // Flag n0
    sel->getFlaggedNodes()->Add(nMap[0]);
    // Test in-edges retrieval
    {
        ObjectsPtr edgeSet;
        // test in Edges account for flagged nodes
        edgeSet = sel->inOrOutEdges(true, nMap[4]);
        EXPECT_EQ(1, edgeSet->Count());

        // no flag nodes
        sel->setHasMemory(true);
        edgeSet = sel->inOrOutEdges(true, nMap[3]);
        EXPECT_EQ(0, edgeSet->Count());

        // with flagged nodes
        sel->setHasMemory(false);
        edgeSet = sel->inOrOutEdges(true, nMap[3]);
        EXPECT_EQ(1, edgeSet->Count());

        // n1 - n4 and n1 - n2, n0 is flagged
        sel->setHasMemory(true);
        edgeSet = sel->inOrOutEdges(true, nMap[1]);
        EXPECT_EQ(2, edgeSet->Count());

        // All neighbors, n0 - n2 is in the 1 hop radius
        sel->setHasMemory(false);
        edgeSet = sel->inOrOutEdges(true, nMap[1]);
        EXPECT_EQ(4, edgeSet->Count());

        sel->setHasMemory(true);
        edgeSet = sel->inOrOutEdges(true, nMap[2]);
        EXPECT_EQ(1, edgeSet->Count());
    }

    // Test out-edges retrieval
    {
        ObjectsPtr edgeSet;
        sel->setHasMemory(true);
        edgeSet = sel->inOrOutEdges(false, nMap[3]);
        EXPECT_EQ(0, edgeSet->Count());

        sel->setHasMemory(false);
        edgeSet = sel->inOrOutEdges(false, nMap[3]);
        EXPECT_EQ(2, edgeSet->Count());

        // Only n1 - n2, n1 - n0 is flagged
        sel->setHasMemory(true);
        edgeSet = sel->inOrOutEdges(false, nMap[4]);
        EXPECT_EQ(1, edgeSet->Count());

        // n1 - n2, n1 - n0
        sel->setHasMemory(false);
        edgeSet = sel->inOrOutEdges(false, nMap[4]);
        EXPECT_EQ(2, edgeSet->Count());
    }

    sel.reset();
    dao.reset();
    sess.reset();
}

TEST( XSelectorTest, rootCentralityScore )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");
    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );

    // Flag n0
    sel->getFlaggedNodes()->Add(nMap[0]);

    double res = -1.0;
    sel->setHasMemory(true);
    res = sel->rootCentralityScore(nMap[4]);
    EXPECT_DOUBLE_EQ(1.0, res);

    // Isolated node because n0 is flagged
    res = sel->rootCentralityScore(nMap[3]);
    EXPECT_DOUBLE_EQ(0.0, res);

    res = sel->rootCentralityScore(nMap[1]);
    EXPECT_DOUBLE_EQ(1.0, res);

    sel->setHasMemory(false);
    res = sel->rootCentralityScore(nMap[3]);
    EXPECT_DOUBLE_EQ(1.0, res);


    // Use flagged
    // traverse edges = 5 + 3 + 1
    // inEdges = traverse edges + n0 - n2 = 18
    res = sel->rootCentralityScore(nMap[1]);
    EXPECT_DOUBLE_EQ(0.5, res);

    sel->getFlaggedNodes()->Remove(nMap[0]);
    sel->getFlaggedNodes()->Add(nMap[4]);

    // do not use flagged nodes
    // traverse edges = 5 + 1
    // inEdges = traverse edges + n0 - n2 = 6 + 9
    sel->setHasMemory(true);
    res = sel->rootCentralityScore(nMap[1]);
    double t = 6 / 15.0;
    EXPECT_DOUBLE_EQ(t, res);

    sel.reset();
    dao.reset();
    sess.reset();
}

TEST( XSelectorTest, twoHubAffinityScore )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");
    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );

    // Flag n0
    sel->getFlaggedNodes()->Add(nMap[0]);

    double res = -1.0;
    // No out edges
    sel->setHasMemory(true);
    res = sel->twoHopHubAffinityScore(nMap[3]);
    EXPECT_DOUBLE_EQ(1.0, res);

    res = sel->twoHopHubAffinityScore(nMap[0]);
    EXPECT_DOUBLE_EQ(1.0, res);

    // Count flagged n0
    sel->setHasMemory(false);
    res = sel->twoHopHubAffinityScore(nMap[3]);
    EXPECT_DOUBLE_EQ(2.0, res);

    res = sel->twoHopHubAffinityScore(nMap[0]);
    EXPECT_DOUBLE_EQ(1.0, res);

    sel.reset();
    dao.reset();
    sess.reset();
}

TEST( XSelectorTest, gravityScore )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");
    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );

    // Flag n0
    sel->getFlaggedNodes()->Add(nMap[0]);

    double res = -1.0;
    sel->setHasMemory(true);
    // No out edges
    res = sel->gravityScore(nMap[3]);
    EXPECT_DOUBLE_EQ(1.0, res);
    res = sel->gravityScore(nMap[0]);
    EXPECT_DOUBLE_EQ(103.0, res);

    // Count flagged n0
    sel->setHasMemory(false);
    res = sel->gravityScore(nMap[3]);
    EXPECT_DOUBLE_EQ(2.0, res);

    res = sel->gravityScore(nMap[0]);
    EXPECT_DOUBLE_EQ(103.0, res);

    sel.reset();
    dao.reset();
    sess.reset();
}

TEST( XSelectorTest, calcScore )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");
    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );

    // Flag n0
    sel->getFlaggedNodes()->Add(nMap[0]);

    double res = -1.0;
    sel->setHasMemory(true);
    // Root centrality is 0
    res = sel->calcScore(nMap[3]);
    EXPECT_DOUBLE_EQ(0.0, res);

    // 1 / (1 * 102)
    res = sel->calcScore(nMap[1]);
    EXPECT_DOUBLE_EQ(double(1/102.0), res);

    sel->setHasMemory(false);
    // 1 / (2 * 2)
    res = sel->calcScore(nMap[3]);
    EXPECT_DOUBLE_EQ(0.25, res);

    // 0.5 / (1 * 103)
    res = sel->calcScore(nMap[1]);
    EXPECT_DOUBLE_EQ(double(1/206.0), res);

    sel.reset();
    dao.reset();
    sess.reset();
}

TEST( XSelectorTest, rankNodes )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");
    SessionPtr sess = sparkseeManager.newSession();
    sparksee::gdb::Graph* g = sess->GetGraph();
    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<XSelector> sel( new XSelector(g) );
    sel->setHasMemory(true);

    Layer base = dao->baseLayer();
    EXPECT_TRUE(sel->rankNodes(base));


    // Only 5 node in graph
    for( int i = 0; i < 5; i++ ) {
        Node n = sel->next();
        EXPECT_NE(sparksee::gdb::Objects::InvalidOID, n.id());
    }
    // Invalid node
    Node n = sel->next();
    EXPECT_EQ(sparksee::gdb::Objects::InvalidOID, n.id());

    EXPECT_TRUE(sel->rankNodes(base));
    sel.reset();
    dao.reset();
    sess.reset();
}

