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

#include <dex/gdb/Dex.h>
#include <dex/gdb/Database.h>
#include <dex/gdb/Session.h>
#include <dex/gdb/Graph.h>
#include <dex/gdb/Value.h>

#include "mld/DexManager.h"
#include "mld/Graph_types.h"

using namespace mld;
using namespace dex::gdb;

DexManager::DexManager( const dex::gdb::DexConfig& conf )
    : m_dex( new dex::gdb::Dex(conf) )
{
}

DexManager::DexManager( const std::wstring& configFile )
{
    dex::gdb::DexProperties::Load(configFile);
    dex::gdb::DexConfig cfg;
    m_dex.reset( new dex::gdb::Dex(cfg) );
}

DexManager::~DexManager()
{
    // All Sessions should be delete first
    m_db.reset();
    m_dex.reset();
}

void DexManager::createDatabase( const std::wstring& path, const std::wstring& alias )
{
    m_db.reset( m_dex->Create(path, alias) );
}

void DexManager::openDatabase( const std::wstring& path, bool read )
{
    m_db.reset( m_dex->Open(path, read) );
}

void DexManager::restoreDatabase( const std::wstring& path, const std::wstring& backupFile )
{
    m_db.reset( m_dex->Restore(path, backupFile) );
}

SessionPtr DexManager::newSession()
{
    return SessionPtr( m_db->NewSession() );
}

void DexManager::createScheme( Graph* g )
{
    createNodeTypes(g);
    createEdgeTypes(g);
}

void DexManager::createNodeTypes( Graph* g )
{
    dex::gdb::Value val;
    attr_t attr;
    // Create SuperNode type
    type_t nType = g->FindType(NodeType::SUPERNODE);
    if( nType != Type::InvalidType ) {
        nType = g->NewNodeType(NodeType::SUPERNODE);
        attr = g->NewAttribute(nType, SNAttr::WEIGHT, Double, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetDouble(0));
    }

    // Create Layer type
    nType = g->FindType(NodeType::LAYER);
    if( nType != Type::InvalidType ) {
        g->NewNodeType(NodeType::LAYER);
        attr = g->NewAttribute(nType, LayerAttr::IS_BASE, Boolean, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetBoolean(false));
        attr = g->NewAttribute(nType, LayerAttr::DESCRIPTION, String, Basic);
        g->SetAttributeDefaultValue(attr, val.SetString(L""));
    }
}

void DexManager::createEdgeTypes( Graph* g )
{
    dex::gdb::Value val;
    attr_t attr;
    // Create H_LINK type
    type_t eType = g->FindType(EdgeType::H_LINK);
    if( eType != Type::InvalidType ) {
        // Undirected, index neighbors
        eType = g->NewEdgeType(EdgeType::H_LINK, false, true);
        attr = g->NewAttribute(eType, H_LinkAttr::WEIGHT, Double, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetDouble(0));
    }

    // Create V_LINK type
    eType = g->FindType(EdgeType::V_LINK);
    if( eType != Type::InvalidType ) {
        // Directed, index neighbors
        eType = g->NewEdgeType(EdgeType::V_LINK, true, true);
        attr = g->NewAttribute(eType, V_LinkAttr::WEIGHT, Double, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetDouble(0));
    }

    // Create OWNS type
    eType = g->FindType(EdgeType::OWNS);
    if( eType != Type::InvalidType ) {
        // Directed, index neighbors
        g->NewEdgeType(EdgeType::OWNS, true, true);
    }

    // Create PARENT_OF type
    eType = g->FindType(EdgeType::PARENT_OF);
    if( eType != Type::InvalidType ) {
        // Directed, index neighbors
        g->NewEdgeType(EdgeType::PARENT_OF, true, true);
    }

    // Create CHILD_OF type
    eType = g->FindType(EdgeType::CHILD_OF);
    if( eType != Type::InvalidType ) {
        // Directed, index neighbors
        g->NewEdgeType(EdgeType::CHILD_OF, true, true);
    }
}


