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

#include <sparksee/gdb/Sparksee.h>
#include <sparksee/gdb/Database.h>
#include <sparksee/gdb/Session.h>
#include <sparksee/gdb/Graph.h>
#include <sparksee/gdb/Value.h>

#include "mld/SparkseeManager.h"
#include "mld/Graph_types.h"

using namespace mld;
using namespace sparksee::gdb;

SparkseeManager::SparkseeManager( const sparksee::gdb::SparkseeConfig& conf )
    : m_sparksee( new sparksee::gdb::Sparksee(conf) )
{
}

SparkseeManager::SparkseeManager( const std::wstring& configFile )
{
    sparksee::gdb::SparkseeProperties::Load(configFile);
    sparksee::gdb::SparkseeConfig cfg;
    m_sparksee.reset( new sparksee::gdb::Sparksee(cfg) );
}

SparkseeManager::~SparkseeManager()
{
    // All Sessions should be delete first
    m_db.reset();
    m_sparksee.reset();
}

void SparkseeManager::createDatabase( const std::wstring& path, const std::wstring& alias )
{
    m_db.reset( m_sparksee->Create(path, alias) );
}

void SparkseeManager::openDatabase( const std::wstring& path, bool readOnly )
{
    m_db.reset( m_sparksee->Open(path, readOnly) );
}

void SparkseeManager::restoreDatabase( const std::wstring& path, const std::wstring& backupFile )
{
    m_db.reset( m_sparksee->Restore(path, backupFile) );
}

SessionPtr SparkseeManager::newSession()
{
    return SessionPtr( m_db->NewSession() );
}

void SparkseeManager::createScheme( Graph* g )
{
    createNodeTypes(g);
    createEdgeTypes(g);
}

void SparkseeManager::createNodeTypes( Graph* g )
{
    sparksee::gdb::Value val;
    attr_t attr;
    // Create SuperNode type
    type_t nType = g->FindType(NodeType::SUPERNODE);
    if( nType == Type::InvalidType ) {
        nType = g->NewNodeType(NodeType::SUPERNODE);
        // WEIGHT
        attr = g->NewAttribute(nType, SNAttr::WEIGHT, Double, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetDouble(kSUPERNODE_DEF_VALUE));
        // LABEL
        attr = g->NewAttribute(nType, SNAttr::LABEL, String, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetString(L""));
        // IS_ROOT
        attr = g->NewAttribute(nType, SNAttr::IS_ROOT, Boolean, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetBoolean(false));
    }

    // Create Layer type
    nType = g->FindType(NodeType::LAYER);
    if( nType == Type::InvalidType ) {
        nType = g->NewNodeType(NodeType::LAYER);
        attr = g->NewAttribute(nType, LayerAttr::IS_BASE, Boolean, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetBoolean(false));
        attr = g->NewAttribute(nType, LayerAttr::DESCRIPTION, String, Basic);
        g->SetAttributeDefaultValue(attr, val.SetString(L""));
    }
}

void SparkseeManager::createEdgeTypes( Graph* g )
{
    sparksee::gdb::Value val;
    attr_t attr;
    // Create H_LINK type
    type_t eType = g->FindType(EdgeType::H_LINK);
    if( eType == Type::InvalidType ) {
        // Undirected, insparksee neighbors
        eType = g->NewEdgeType(EdgeType::H_LINK, false, true);
        attr = g->NewAttribute(eType, H_LinkAttr::WEIGHT, Double, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetDouble(kHLINK_DEF_VALUE));
    }

    // Create V_LINK type
    eType = g->FindType(EdgeType::V_LINK);
    if( eType == Type::InvalidType ) {
        // Directed, insparksee neighbors
        eType = g->NewEdgeType(EdgeType::V_LINK, true, true);
        attr = g->NewAttribute(eType, V_LinkAttr::WEIGHT, Double, Indexed);
        g->SetAttributeDefaultValue(attr, val.SetDouble(kVLINK_DEF_VALUE));
    }

    // Create OWNS type
    eType = g->FindType(EdgeType::OWNS);
    if( eType == Type::InvalidType ) {
        // Directed, insparksee neighbors
        g->NewEdgeType(EdgeType::OWNS, true, true);
    }

    // Create CHILD_OF type
    eType = g->FindType(EdgeType::CHILD_OF);
    if( eType == Type::InvalidType ) {
        // Directed, insparksee neighbors
        g->NewEdgeType(EdgeType::CHILD_OF, true, true);
    }
}


