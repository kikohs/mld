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

#include <locale>
#include <codecvt>

#include <mld/config.h>
#include <mld/SparkseeManager.h>
#include <boost/property_map/property_map.hpp>

#include <mld/dao/MLGDao.h>
#include <mld/analytics/ComponentExtractor.h>
#include <mld/model/DynamicGraph.h>
#include <mld/io/GraphExporter.h>

using namespace mld;
using namespace sparksee::gdb;

TEST( ComponentExtractorTest, ExtractComponentGraph )
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
    mld::Node n1 = dao->addNodeToLayer(base); // disabled
    mld::Node n2 = dao->addNodeToLayer(base); // disabled
    mld::Node n3 = dao->addNodeToLayer(base); // disabled

    dao->addHLink(n1, n2);
    dao->addHLink(n1, n3);

    Layer l2 = dao->addLayerOnTop();

    AttrMap data;
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(5);
    dao->addOLink(l2, n1, data);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(-5);
    dao->addOLink(l2, n2, data);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(1);
    dao->addOLink(l2, n3, data); // disabled

    Layer l3 = dao->addLayerOnTop();
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(6);
    dao->addOLink(l3, n1, data);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(-8);
    dao->addOLink(l3, n2, data);
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(-1);
    dao->addOLink(l3, n3, data); // disabled

    // No olink above threshold
    Layer l4 = dao->addLayerOnTop();
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(1);
    dao->addOLink(l4, n1, data);  // disabled
    dao->addOLink(l4, n2, data); // disabled
    data[Attrs::V[OLinkAttr::WEIGHT]].SetDoubleVoid(7);
    dao->addOLink(l4, n3, data);

    // Use shared instead of unique to have QtCreator linter ...
    std::shared_ptr<ComponentExtractor> extract(new ComponentExtractor(g));

    // Auto threshold should be at 3.5
    bool ok = extract->run();
    EXPECT_TRUE(ok);

    DynGraphPtr vgraph = extract->dynGraph();
    if( !vgraph ) {
        LOG(logERROR) << "Vgraph is null";
        EXPECT_TRUE(false);
        return;
    }

    auto d = vgraph->data();
    EXPECT_EQ(uint32_t(5), boost::num_vertices(d));
    EXPECT_EQ(uint32_t(5), boost::num_edges(d));

    // Node n2_l2 should be in the graph
    ok = vgraph->exist(l4.id(), n3.id());
    EXPECT_TRUE(ok);

    // Valid ids but under threshold
    ok = vgraph->exist(l4.id(), n2.id());
    EXPECT_FALSE(ok);

    auto vn2 = vgraph->getNode(l2.id(), n2.id());
    EXPECT_TRUE(vn2.second);
    EXPECT_DOUBLE_EQ(-5, vn2.first.weight());

    // Test with invalid data
    auto vnInvalid = vgraph->getNode(8, n2.id());
    EXPECT_FALSE(vnInvalid.second);

    vnInvalid = vgraph->getNode(l4.id(), 56);
    EXPECT_FALSE(vnInvalid.second);


    // Export
    using Converter = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>;
    Converter converter;

    ok = GraphExporter::exportDynamicGraphAsJson(vgraph, converter.to_bytes(mld::kRESOURCES_DIR) + "mld_vgraph_test.json");
    EXPECT_TRUE(ok);

    extract.reset();
    dao.reset();
    sess.reset();
}
