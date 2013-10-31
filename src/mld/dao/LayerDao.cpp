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

#include <dex/gdb/Graph.h>
#include <dex/gdb/Objects.h>
#include <dex/gdb/Graph_data.h>
#include <dex/gdb/Value.h>

#include <boost/log/trivial.hpp>

#include "mld/Graph_types.h"
#include "mld/dao/LayerDao.h"

using namespace mld;
using namespace dex::gdb;

LayerDao::LayerDao( dex::gdb::Graph* g )
    : m_g(g)
    , m_v( new dex::gdb::Value )
    , m_lType(m_g->FindType(NodeType::LAYER))
{
}

LayerDao::~LayerDao()
{
    // DO NOT DELETE GRAPH
}

int LayerDao::countLayers()
{

}

Layer LayerDao::addLayerOnTop()
{

}

Layer LayerDao::addLayerOnBottom()
{

}

void LayerDao::updateLayer( const Layer& layer )
{

}

Layer LayerDao::getLayer( dex::gdb::oid_t id )
{

}

void LayerDao::removeTopLayer()
{

}

void LayerDao::removeBottomLayer()
{

}

Layer LayerDao::bottomLayer()
{

}

Layer LayerDao::topLayer()
{

}

void LayerDao::setAsBaseLayer( Layer& layer )
{

}

// PRIVATE METHODS
Layer LayerDao::addLayer()
{
    return Layer( m_g->NewNode(m_lType) );
}

void LayerDao::removeLayer( const Layer& layer )
{

}

dex::gdb::oid_t LayerDao::getBaseLayerImpl()
{
    auto attr = m_g->FindAttribute(m_lType, LayerAttr::IS_BASE);

    std::unique_ptr<Objects> obj = m_g->Select(attr, Equal, m_v->SetBoolean(true));
    if( obj->Count() == 0 ) {
        BOOST_LOG_TRIVIAL(warning) << "LayerDao::baseLayerImpl: No base layer";
        return Objects::InvalidOID;
    }
    // Should only contain 1 element
    return obj->Any();
}

void LayerDao::setAsBaseLayerImpl( dex::gdb::oid_t newId )
{
    auto attr = m_g->FindAttribute(m_lType, LayerAttr::IS_BASE);
    auto oldId = getBaseLayerImpl();
    // No base layer
    if( oldId == Objects::InvalidOID ) {
        m_g->SetAttribute(newId, attr, m_v->SetBoolean(true));
    }
    else { // There is already a base layer, switch
        m_g->SetAttribute(oldId, attr, m_v->SetBoolean(false));
        m_g->SetAttribute(newId, attr, m_v->SetBoolean(true));
    }
}


