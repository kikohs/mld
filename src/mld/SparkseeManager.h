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

#ifndef MLD_SPARKSEEMANAGER_H
#define MLD_SPARKSEEMANAGER_H

/**
 * @addtogroup mld
 * @{
 *
 * @file SparkseeManager.h
 * @brief Manage Sparksee graph database. Gives Session for clients.
 *
 * @author Kirell
 */

#include "mld/common.h"
#include "mld/Session.h"

namespace sparksee {
namespace gdb {
    class Sparksee;
    class Database;
    class SparkseeConfig;
    class Value;
}}

namespace mld {

class MLD_API SparkseeManager
{
    typedef std::unique_ptr<sparksee::gdb::Sparksee> SparkseePtr;
    typedef std::unique_ptr<sparksee::gdb::Database> DatabasePtr;

public:
    /**
     * @brief SparkseeManager constructor
     * @param conf. SparkseeConfig object
     */
    SparkseeManager( const sparksee::gdb::SparkseeConfig& conf );
    // Disable copy and assignement constructors
    SparkseeManager( const SparkseeManager& ) = delete;
    SparkseeManager& operator=( const SparkseeManager& ) = delete;

    /**
     * @brief SparkseeManager second constructor
     * @param configFile, path to configFile. Loaded to configure sparksee.
     */
    SparkseeManager( const std::wstring& configFile );

    /**
     * Destructor, Sparksee manager should be deleted last
     * when all the SessionPtr are deleted (reset)
     */
    ~SparkseeManager();

    /**
     * @brief Creates a new Database instance.
     * @param path [in] Database storage file.
     * @param alias [in] Database alias name.
     * @exception sparksee::gdb::FileNotFounsparkseeception
     * If the given file cannot be created.
     */
    void createDatabase( const std::wstring& path, const std::wstring& alias );

    /**
     * @brief Opens an existing Database instance.
     * @param path [in] Database storage file.
     * @param read [in] If TRUE, open Database in read-only mode.
     * @exception sparksee::gdb::FileNotFounsparkseeception If the given file does not exist.
     */
    void openDatabase( const std::wstring& path, bool readOnly = false );

    /**
     * @brief Restores a Database from a backup file. See Graph::Backup.
     * @param path [in] Database storage file.
     * @param backupFile [in] The Backup file to be restored.
     * @exception sparksee::gdb::FileNotFounsparkseeception
     * If the given file cannot be created, or the exported data file does not exists.
     */
    void restoreDatabase( const std::wstring& path, const std::wstring& backupFile );

    /**
     * @brief Creates a new Session from the Database
     */
    SessionPtr newSession();

    /**
     * @brief Get a view of the database
     * @return sparksee database, DO NOT DELETE
     */
    sparksee::gdb::Database* database() { return m_db.get(); }

    /**
     * @brief Create databse scheme, Nodes and relation types
     */
    void createBaseScheme( sparksee::gdb::Graph* g );

    /**
     * @brief Rename default attributes
     * @param objType, NODE or EDGE
     * @param key GraphTypes enum
     * @param newName
     */
    static void renameDefaultAttribute( sparksee::gdb::Graph* g,
                                 const std::wstring& objType,
                                 int key,
                                 const std::wstring& newName );

    /**
     * @brief Add an attribute to the SuperNode type
     * @param g Graph
     * @param key Key to retrieve the attribute
     * @param dtype Type of the attribute
     * @param aKind Index or Basic
     * @param defaultValue
     * @return success
     */
    static bool addAttrToNode( sparksee::gdb::Graph* g,
                        const std::wstring& key,
                        sparksee::gdb::DataType dtype,
                        sparksee::gdb::AttributeKind aKind,
                        sparksee::gdb::Value& defaultValue );

    /**
     * @brief Add an attribute to the HLink type
     * @param g Graph
     * @param key Key to retrieve the attribute
     * @param dtype Type of the attribute
     * @param aKind Index or Basic
     * @param defaultValue
     * @return success
     */
    static bool addAttrToHLink( sparksee::gdb::Graph* g,
                         const std::wstring& key,
                         sparksee::gdb::DataType dtype,
                         sparksee::gdb::AttributeKind aKind,
                         sparksee::gdb::Value& defaultValue );

    /**
     * @brief Add an attribute to the VLink type
     * @param g Graph
     * @param key Key to retrieve the attribute
     * @param dtype Type of the attribute
     * @param aKind Index or Basic
     * @param defaultValue
     * @return success
     */
    static bool addAttrToVLink( sparksee::gdb::Graph* g,
                         const std::wstring& key,
                         sparksee::gdb::DataType dtype,
                         sparksee::gdb::AttributeKind aKind,
                         sparksee::gdb::Value& defaultValue );

    /**
     * @brief Add an attribute to the Owns type
     * @param g Graph
     * @param key Key to retrieve the attribute
     * @param dtype Type of the attribute
     * @param aKind Index or Basic
     * @param defaultValue
     * @return success
     */
    static bool addAttrToOLink( sparksee::gdb::Graph* g,
                            const std::wstring& key,
                            sparksee::gdb::DataType dtype,
                            sparksee::gdb::AttributeKind aKind,
                            sparksee::gdb::Value& defaultValue );
private:
    /**
     * @brief Create NodeType if not already in the db
     * @param g
     */
    void createNodeTypes( sparksee::gdb::Graph *g );
    void createEdgeTypes( sparksee::gdb::Graph* g );

    static bool addAttr( sparksee::gdb::Graph* g,
                  const std::wstring& ObjectKey,
                  const std::wstring& key,
                  sparksee::gdb::DataType dtype,
                  sparksee::gdb::AttributeKind aKind,
                  sparksee::gdb::Value& defaultValue );

private:
    SparkseePtr m_sparksee;
    DatabasePtr m_db;
};

} // end namespace mld

/** @} */
#endif // MLD_SPARKSEEMANAGER_H
