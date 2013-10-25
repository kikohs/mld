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

#ifndef MLD_DEXMANAGER_H
#define MLD_DEXMANAGER_H

/**
 * @addtogroup mld
 * @{
 *
 * @file DexManager.h
 * @brief Manage Dex graph database. Gives Session for clients.
 *
 * @author Kirell
 */

#include <boost/noncopyable.hpp>

#include "mld/common.h"
#include "mld/Session.h"

namespace dex {
namespace gdb {
    class Dex;
    class Database;
}}

namespace mld {

class MLD_API DexManager: public boost::noncopyable
{
    typedef std::unique_ptr<dex::gdb::Dex> DexPtr;
    typedef std::unique_ptr<dex::gdb::Database> DatabasePtr;

public:
    /**
     * @brief DexManager constructor
     * @param conf. DexConfig object
     */
    DexManager( const dex::gdb::DexConfig& conf );

    /**
     * @brief DexManager second constructor
     * @param configFile, path to configFile. Loaded to configure dex.
     */
    DexManager( const std::wstring& configFile );

    /**
     * Destructor, Dex manager should be deleted last when all the SessionPtr are deleted (reset)
     */
    ~DexManager();

    /**
     * @brief Creates a new Database instance.
     * @param path [in] Database storage file.
     * @param alias [in] Database alias name.
     * @exception dex::gdb::FileNotFoundException If the given file cannot be created.
     */
    void createDatabase( const std::wstring& path, const std::wstring& alias );

    /**
     * @brief Opens an existing Database instance.
     * @param path [in] Database storage file.
     * @param read [in] If TRUE, open Database in read-only mode.
     * @exception dex::gdb::FileNotFoundException If the given file does not exist.
     */
    void openDatabase( const std::wstring& path, bool read );

    /**
     * @brief Restores a Database from a backup file. See Graph::Backup.
     * @param path [in] Database storage file.
     * @param backupFile [in] The Backup file to be restored.
     * @exception dex::gdb::FileNotFoundException If the given file cannot be created, or the exported data file does not exists.
     */
    void restoreDatabase( const std::wstring& path, const std::wstring& backupFile );

    /**
     * @brief Creates a new Session from the Database
     */
    SessionPtr newSession();

    /**
     * @brief Get a view of the database
     * @return dex database, DO NOT DELETE
     */
    dex::gdb::Database* database() { return m_db.get(); }

private:
    DexPtr m_dex;
    DatabasePtr m_db;
};

} // end namespace mld

/** @} */
#endif // MLD_DEXMANAGER_H
