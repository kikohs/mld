/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_DATABASE_H_
#define _SPARKSEE_DATABASE_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Database.h
 * \brief It contains the definition of Database class as well as some other
 * related classes.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/Handler.h"

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class GraphPool;
}

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 * Forward declaration of sparksee classes.
 */
class Sparksee;
class Database;
class Session;
class Graph;


/**
 * \brief Database statistics.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT DatabaseStatistics
{
    friend class Database; // to set private fields
    
private:
    /**
     * \brief Total read data in KBytes.
     */
    int64_t read;
    
    /**
     * \brief Total written data in KBytes.
     */
    int64_t write;
    
    /**
     * \brief Database size in KBytes.
     */
    int64_t data;
    
    /**
     * \brief Cache size in KBytes.
     */
    int64_t cache;
    
    /**
     * \brief Temporary storage file size in KBytes.
     */
    int64_t temp;
    
    /**
     * \brief Number of sessions.
     */
    int64_t sessions;
    
public:
    
    /**
     * \brief Gets total read data in KBytes.
     * \return Total read data in KBytes.
     */
    int64_t GetRead() const 
    {
        return read;
    }
    
    /**
     * \brief Gets total written data in KBytes.
     * \return Total read written in KBytes.
     */
    int64_t GetWrite() const 
    {
        return write;
    }
    
    /**
     * \brief Gets database size in KBytes.
     * \return Database size in KBytes.
     */
    int64_t GetData() const 
    {
        return data;
    }
    
    /**
     * \brief Gets cache size in KBytes.
     * \return Cache size in KBytes.
     */
    int64_t GetCache() const 
    {
        return cache;
    }
    
    /**
     * \brief Gets temporary storage file size in KBytes.
     * \return Temporary storage file size in KBytes.
     */
    int64_t GetTemp() const 
    {
        return temp;
    }
    
    /**
     * \brief Gets the number of sessions.
     * \return The number of sessions.
     */
    int64_t GetSessions() const 
    {
        return sessions;
    }
};


/**
 * \brief Database class.
 *
 * All the data of the Database is stored into a persistent file which
 * just can be created or open through a Sparksee instance.
 *
 * Also, all the manipulation of a Database must be done by means of a Session
 * which can be initiated from a Database instance.
 *
 * Multiple Databases do not share the memory, that is there is no negotiation 
 * among them. In those cases, memory must be prefixed for each Database.
 * To do that, use the SPARKSEEConfig.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Database
: private Handler<sparksee_core::GraphPool>
{
    friend class Sparksee; // to access private constructor
    friend class Graph; // to access sparksee_core handled reference
    
public:
    
    /**
     * \brief Destructor.
     */
    virtual ~Database();
    
    /**
     * \brief Gets the alias of the Database.
     * \return The alias of the Database.
     */
    const std::wstring & GetAlias() const
    {
        return alias;
    }
    
    /**
     * \brief Gets the path of the Database.
     * \return The path of the Database.
     */
    const std::wstring & GetPath() const
    {
        return path;
    }
    
    /**
     * \brief Creates a new Session.
     */
    Session * NewSession();
    
    /**
     * \brief Gets Database statistics.
     * \param stats [out] The DatabaseStatistics instance.
     */
    void GetStatistics(DatabaseStatistics & stats);
    
private:
    
    /**
     * \brief Creates a new instance.
     * \param core_gpool [in] Handled sparksee_core GraphPool reference.
     * \param d [in] Parent Sparksee reference.
     */
    Database(sparksee_core::GraphPool * core_gpool, Sparksee * d);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Database(const Database &db);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    Database & operator =(const Database & db);
    
    /**
     * \brief Parent Sparksee reference.
     */
    Sparksee * sparksee;
    
    /**
     * \brief Database alias.
     */
    std::wstring alias;
    
    /**
     * \brief Path alias.
     */
    std::wstring path;
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_DATABASE_H_
