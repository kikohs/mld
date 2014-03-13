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
#include "mld/GraphTypes.h"

using namespace mld;
using namespace sparksee::gdb;

SparkseeManager::SparkseeManager( const SparkseeConfig& conf )
    : m_sparksee( new Sparksee(conf) )
{
}

SparkseeManager::SparkseeManager( const std::wstring& configFile )
{
    SparkseeProperties::Load(configFile);
    SparkseeConfig cfg;
    m_sparksee.reset( new Sparksee(cfg) );
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

void SparkseeManager::createBaseScheme( Graph* g )
{
    createNodeTypes(g);
    createEdgeTypes(g);
}

bool SparkseeManager::addAttrToNode( Graph* g,
                                              const std::wstring& key,
                                              DataType dtype,
                                              AttributeKind aKind,
                                              Value& defaultValue )
{
    bool ok = addAttr(g, NodeType::NODE, key, dtype, aKind, defaultValue);
    if( !ok ) {
        LOG(logERROR) << "SparkseeManager::addAttrToNode SuperNode nodeType not created";
    }
    return ok;
}

bool SparkseeManager::addAttrToHLink( Graph* g,
                                          const std::wstring& key,
                                          DataType dtype,
                                          AttributeKind aKind,
                                          Value& defaultValue )
{
    bool ok = addAttr(g, EdgeType::H_LINK, key, dtype, aKind, defaultValue);
    if( !ok ) {
        LOG(logERROR) << "SparkseeManager::addAttrToHLink HLink edgeType not created";
    }
    return ok;
}

bool SparkseeManager::addAttrToVLink( Graph* g,
                                          const std::wstring& key,
                                          DataType dtype,
                                          AttributeKind aKind,
                                          Value& defaultValue )
{
    bool ok = addAttr(g, EdgeType::V_LINK, key, dtype, aKind, defaultValue);
    if( !ok ) {
        LOG(logERROR) << "SparkseeManager::addAttrToVLink HLink edgeType not created";
    }
    return ok;
}

// Private

void SparkseeManager::createNodeTypes( Graph* g )
{
    Value val;
    // Create SuperNode type
    type_t nType = g->FindType(NodeType::NODE);
    if( nType == Type::InvalidType ) {
        nType = g->NewNodeType(NodeType::NODE);
        addAttr(g, NodeType::NODE, NodeAttr::WEIGHT, Double, Indexed, val.SetDouble(kNODE_DEF_VALUE));
        addAttr(g, NodeType::NODE, NodeAttr::LABEL, String, Indexed, val.SetString(L""));
    }

    // Create Layer type
    nType = g->FindType(NodeType::LAYER);
    if( nType == Type::InvalidType ) {
        nType = g->NewNodeType(NodeType::LAYER);
        addAttr(g, NodeType::LAYER, LayerAttr::IS_BASE, Boolean, Indexed, val.SetBoolean(false));
        addAttr(g, NodeType::LAYER, LayerAttr::DESCRIPTION, String, Basic, val.SetString(L""));
    }
}

void SparkseeManager::createEdgeTypes( Graph* g )
{
    Value val;
    // Create H_LINK type
    type_t eType = g->FindType(EdgeType::H_LINK);
    if( eType == Type::InvalidType ) {
        // Undirected, insparksee neighbors
        eType = g->NewEdgeType(EdgeType::H_LINK, false, true);
        addAttr(g, EdgeType::H_LINK, H_LinkAttr::WEIGHT, Double, Indexed, val.SetDouble(kHLINK_DEF_VALUE));
    }

    // Create V_LINK type
    eType = g->FindType(EdgeType::V_LINK);
    if( eType == Type::InvalidType ) {
        // Directed, insparksee neighbors
        eType = g->NewEdgeType(EdgeType::V_LINK, true, true);
        addAttr(g, EdgeType::V_LINK, V_LinkAttr::WEIGHT, Double, Indexed, val.SetDouble(kVLINK_DEF_VALUE));
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

bool SparkseeManager::addAttr( Graph* g,
                               const std::wstring& ObjectKey,
                               const std::wstring& key,
                               DataType dtype,
                               AttributeKind aKind,
                               Value& defaultValue )
{
    type_t nType = g->FindType(ObjectKey);
    if( nType == Type::InvalidType ) {
        return false;
    }
    auto attr = g->NewAttribute(nType, key, dtype, aKind);
    if( !defaultValue.IsNull() )
        g->SetAttributeDefaultValue(attr, defaultValue);
    return true;
}

