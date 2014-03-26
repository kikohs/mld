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
    Layer middle = dao->addLayerOnTop();
    Layer top = dao->addLayerOnTop();

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

        // mean( n1 + n2 * n12 )
        // mean( 10 + 20 * 0.5 )
        EXPECT_EQ(10.0, ol1.weight());

        // n2
        nid = it->Next();
        OLink ol2 = filter->compute(base.id(), nid);
        // mean( n1 * n12 + n2 + n3 * n23 )
        // mean( 10 * 0.5 + 20 + 40 * 0.1 )
        EXPECT_EQ(29 / 3.0, ol2.weight());

        // n3
        nid = it->Next();
        OLink ol3 = filter->compute(base.id(), nid);
        // mean( n3 + n2 * n23 )
        // mean( 40 + 20 * 0.1 )
        EXPECT_EQ(42 / 2.0, ol3.weight());
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
    filter->computeTWCoeffs(base.id());
    LOG(logINFO) << *filter;

    // Time only
    {
        // TODO

//        ObjectsPtr nodes = dao->getAllNodeIds(base);
//        ObjectsIt it( nodes->Iterator() ); // sorted by oid so n1 is the first node
//        // n1
//        oid_t nid = it->Next();
//        OLink ol1 = filter->compute(base.id(), nid);

//        // mean( n1 + n2 * n12 )
//        // mean( 10 + 20 * 0.5 )
//        EXPECT_EQ(10.0, ol1.weight());

//        // n2
//        nid = it->Next();
//        OLink ol2 = filter->compute(base.id(), nid);
//        // mean( n1 * n12 + n2 + n3 * n23 )
//        // mean( 10 * 0.5 + 20 + 40 * 0.1 )
//        EXPECT_EQ(29 / 3.0, ol2.weight());

//        // n3
//        nid = it->Next();
//        OLink ol3 = filter->compute(base.id(), nid);
//        // mean( n3 + n2 * n23 )
//        // mean( 40 + 20 * 0.1 )
//        EXPECT_EQ(42 / 2.0, ol3.weight());
    }

    dao.reset();
    filter.reset();
    sess.reset();
}
