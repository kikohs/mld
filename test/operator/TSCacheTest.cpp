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
#include <mld/SparkseeManager.h>

#include <mld/dao/MLGDao.h>
#include <mld/operator/TSCache.h>

using namespace mld;
using namespace sparksee::gdb;

TEST( TSCacheTest, CRUD )
{
    mld::SparkseeManager sparkseeManager(mld::kRESOURCES_DIR + L"mysparksee.cfg");
    sparkseeManager.createDatabase(mld::kRESOURCES_DIR + L"MLDTest.sparksee", L"MLDTest");

    SessionPtr sess = sparkseeManager.newSession();
    Graph* g = sess->GetGraph();
    // Create Db scheme
    sparkseeManager.createBaseScheme(g);

    std::shared_ptr<MLGDao> dao( new MLGDao(g) );
    Layer base = dao->addBaseLayer();

    // Add OLinks
    mld::Node n1 = dao->addNodeToLayer(base);
    mld::Node n2 = dao->addNodeToLayer(base);
    Layer l2 = dao->addLayerOnTop();
    AttrMap data;
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(2);
    dao->addOLink(l2, n1, data);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(12);
    dao->addOLink(l2, n2, data);

    Layer l3 = dao->addLayerOnTop();
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(3);
    dao->addOLink(l3, n1, data);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(13);
    dao->addOLink(l3, n2, data);

    Layer l4 = dao->addLayerOnTop();
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(4);
    dao->addOLink(l4, n1, data);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(14);
    dao->addOLink(l4, n2, data);

    Layer l5 = dao->addLayerOnTop();
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(5);
    dao->addOLink(l5, n1, data);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(15);
    dao->addOLink(l5, n2, data);

    TSCache cache(dao);
    // TODO
}
