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

#include "DexManager.h"

using namespace mld;

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


