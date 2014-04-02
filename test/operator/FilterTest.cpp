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

//#include <sparksee/gdb/Graph.h>
//#include <sparksee/gdb/Objects.h>
//#include <sparksee/gdb/ObjectsIterator.h>

#include <mld/common.h>
#include <mld/config.h>
#include <mld/SparkseeManager.h>

#include <mld/operator/filters.h>
#include <mld/dao/MLGDao.h>

using namespace mld;
using namespace sparksee::gdb;

void createDatabase()
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    Layer base = dao->addBaseLayer();
//    LOG(logDEBUG) << base;
    Layer middle = dao->addLayerOnTop();
//    LOG(logDEBUG) << middle;
    Layer top = dao->addLayerOnTop();
//    LOG(logDEBUG) << top;

    AttrMap nodeData;
    AttrMap oLinkData;
    oLinkData[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(10.0);
    // Add note to 3 layers with weight = 10.0
    mld::Node n1 = dao->addNodeToLayer(base, nodeData, oLinkData);
    dao->addOLink(middle, n1, oLinkData);
    dao->addOLink(top, n1, oLinkData);

    oLinkData[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(20.0);
    mld::Node n2 = dao->addNodeToLayer(base, nodeData, oLinkData);
    dao->addOLink(middle, n2, oLinkData);
    dao->addOLink(top, n2, oLinkData);

    dao->addHLink(n1, n2, 0.5);

    oLinkData[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(40.0);
    mld::Node n3 = dao->addNodeToLayer(base, nodeData, oLinkData);
    oLinkData[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(80.0);
    dao->addOLink(middle, n3, oLinkData);
    oLinkData[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(160.0);
    dao->addOLink(top, n3, oLinkData);

    dao->addHLink(n2, n3, 0.1);

    // n1-n2 = 0.5
    // n2-n3 = 0.1;
    // n1 = n1m = n1t = 10
    // n2 = n2m = n2t = 20
    // n3 = 40; n3m = 80; n3t = 160
    //
    //  o -- o -- o   top
    //  |    |    |
    //  o -- o -- o   middle
    //  |    |    |
    //  o -- o -- o   base
    //  n1   n2   n3

    dao.reset();
    sess.reset();
}

TEST( FilterTest, TVMFilterVertexOnly )
{
    createDatabase();
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();

    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<TimeVertexMeanFilter> filter( new TimeVertexMeanFilter(g) );

    Layer base = dao->baseLayer();
    filter->computeTWCoeffs(base.id());
    LOG(logINFO) << *filter;

    // Vertex domain only
    {
        ObjectsPtr nodes = dao->getAllNodeIds(base);
        ObjectsIt it( nodes->Iterator() ); // sorted by oid so n1 is the first node
        // n1
        oid_t nid = it->Next();
        OLink ol1 = filter->compute(base.id(), nid);
        auto olinks = dao->getAllOLinks(nid);
        EXPECT_EQ(size_t(3), olinks.size());

        // value = n1 + n2 * n12
        // coeff = 1 + n12
        double v = 10 + 20 * 0.5;
        double c = 1.0 + 0.5;
        EXPECT_EQ(v/c, ol1.weight());

        // n2
        nid = it->Next();
        OLink ol2 = filter->compute(base.id(), nid);
        // value = n2 + n1 * n12 + n3 * n23
        // coeff = 1 + n12 + n23;
        v = 20 + 10 * 0.5 + 40 * 0.1;
        c = 1 + 0.5 + 0.1;
        EXPECT_EQ(v/c, ol2.weight());

        // n3
        nid = it->Next();
        OLink ol3 = filter->compute(base.id(), nid);
        // value = n3 + n2 * n23
        // coeff = 1 + n23
        v = 40 + 20 * 0.1;
        c =  1 + 0.1;
        EXPECT_EQ(v/c, ol3.weight());
    }

    dao.reset();
    filter.reset();
    sess.reset();
}

TEST( FilterTest, TVMFilterTimeOnly )
{
    createDatabase();
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();

    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<TimeVertexMeanFilter> filter( new TimeVertexMeanFilter(g) );
    filter->setFilterOnlyInTimeDomain(true);
    filter->setTimeWindowSize(2);

    Layer base = dao->baseLayer();
    Layer middle = dao->parent(base);
    Layer top = dao->parent(middle);

    filter->computeTWCoeffs(base.id());
    LOG(logINFO) << *filter;

    // Time only, lambda = 1
    {
        ObjectsPtr nodes = dao->getAllNodeIds(base);
        ObjectsIt it( nodes->Iterator() ); // sorted by oid so n1 is the first node
        // Base n1
        oid_t nid = it->Next();
        OLink ol1 = filter->compute(base.id(), nid);
        // value = n1 + n1m / (1/l1) + n1t / (1/l1 + 1/l2)
        // coeff = 1 + 1/(1/l1) + 1/(1/l1 + 1/l2)
        double v = 10.0 + 10.0 + 10/2.0;
        double c = 1.0 + 1.0/1.0 + 1.0/(1.0 + 1.0);
        EXPECT_DOUBLE_EQ(v/c, ol1.weight());

        // Top n1t
        filter->computeTWCoeffs(top.id());
        OLink ol1t = filter->compute(top.id(), nid);
        EXPECT_DOUBLE_EQ(ol1.weight(), ol1t.weight());

        // Middle n1m
        filter->computeTWCoeffs(middle.id());
        OLink ol1m = filter->compute(middle.id(), nid);
        // value = n1m + n1 / (1/l1) + n1t / (1/l2)
        // coeff = 1 + 1/(1/l1) + 1/(1/l2)
        v = 10 + 10 + 10;
        c = 1 + 1 + 1;
        EXPECT_DOUBLE_EQ(v/c, ol1m.weight());
    }

    dao.reset();
    filter.reset();
    sess.reset();
}

TEST( FilterTest, TVMFilter1 )
{
    createDatabase();
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.openDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();

    std::unique_ptr<MLGDao> dao( new MLGDao(g) );
    std::unique_ptr<TimeVertexMeanFilter> filter( new TimeVertexMeanFilter(g) );
    filter->setTimeWindowSize(2);

    Layer base = dao->baseLayer();
    Layer middle = dao->parent(base);
    Layer top = dao->parent(middle);

    filter->computeTWCoeffs(base.id());
    LOG(logINFO) << *filter;

    {
        ObjectsPtr nodes = dao->getAllNodeIds(base);
        ObjectsIt it( nodes->Iterator() ); // sorted by oid so n1 is the first node
        // Base n1
        oid_t nid = it->Next();
        OLink ol1 = filter->compute(base.id(), nid);
        // value = n1 + n1m / (1/l1) + n1t / (1/l1 + 1/l2) +
        //         n12 * n2 + n2m / (1/n12 + 1/l1)) + n2t / (1/n12 + 1/l1 + 1/l2)
        // coeff = 1 + 1/(1/l1) + 1/(1/l1 + 1/l2) +
        //         n12 + 1/(1/n12 + 1/l1) + 1/(1/n12 + 1/l1 + 1/l2)
        double v = 10 + 10 + 10/2.0 + 20*0.5 + 20.0/3.0 + 20/4.0;
        double c = 1 + 1 + 1/2.0 + 0.5 + 1/3.0 + 1/4.0;
        EXPECT_DOUBLE_EQ(v/c, ol1.weight());

        // Update lambda weight and switch to n2 node
        nid = it->Next();
        dao->updateCLink(base.id(), middle.id(), 2);
        filter->computeTWCoeffs(middle.id());
        OLink ol2 = filter->compute(middle.id(), nid);
        // value = n2m + n1m*n12 + n3m*n23 +
        //         n2 / (1/l1) + n2t / (1/l2) +
        //         n1 / (1/n12 + 1/l1) + n1t / (1/n12 + 1/l2) +
        //         n3 / (1/n23 + 1/l1) + n3t / (1/n23 + 1/l2)

        // coeff = 1 + n12 + n23 +
        //         1 / (1/l1) + 1 / (1/l2) +
        //         1 / (1/n12 + 1/l1) + 1 / (1/n12 + 1/l2) +
        //         1 / (1/n23 + 1/l1) + 1 / (1/n23 + 1/l2)

        v = 20 + 10.0 * 0.5 + 80.0 * 0.1 +
            20 / (1/2.0) + 20 / 1.0 +
            10 / (1/0.5 + 1/2.0) + 10 / (1/0.5 + 1/1) +
            40 / (1/0.1 + 1/2.0) + 160 / (1/0.1 + 1/1);

        c = 1 + 0.5 + 0.1 +
            1 / 0.5 + 1 +
            1 / (1/0.5 + 1/2.0) + 1 / (1/0.5 + 1/1) +
            1 / (1/0.1 + 1/2.0) + 1 / (1/0.1 + 1/1);
        EXPECT_DOUBLE_EQ(v/c, ol2.weight());

        // Test overriding parameters
        filter->setOverrideInterLayerWeight(true, 1.0);
        filter->computeTWCoeffs(middle.id());
        OLink ol2bis = filter->compute(middle.id(), nid);

        // value = n2m + n1m*n12 + n3m*n23 +
        //         n2 / (1/l1) + n2t / (1/l2) +
        //         n1 / (1/n12 + 1/l1) + n1t / (1/n12 + 1/l2) +
        //         n3 / (1/n23 + 1/l1) + n3t / (1/n23 + 1/l2)

        // coeff = 1 + n12 + n23 +
        //         1 / (1/l1) + 1 / (1/l2) +
        //         1 / (1/n12 + 1/l1) + 1 / (1/n12 + 1/l2) +
        //         1 / (1/n23 + 1/l1) + 1 / (1/n23 + 1/l2)

        v = 20.0 + 10.0 * 0.5 + 80.0 * 0.1 +
            20.0 / 1.0 + 20.0 / 1.0 +
            10.0 / (1/0.5 + 1/1.0) + 10.0 / (1/0.5 + 1/1.0) +
            40.0 / (1/0.1 + 1/1.0) + 160.0 / (1/0.1 + 1/1.0);

        c = 1 + 0.5 + 0.1 +
            1 + 1 +
            1 / (1/0.5 + 1/1.0) + 1 / (1/0.5 + 1/1) +
            1 / (1/0.1 + 1/1.0) + 1 / (1/0.1 + 1/1);
        EXPECT_DOUBLE_EQ(v/c, ol2bis.weight());
    }

    dao.reset();
    filter.reset();
    sess.reset();
}
