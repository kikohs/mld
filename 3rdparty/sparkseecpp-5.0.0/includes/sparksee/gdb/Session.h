/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_SESSION_H_
#define _SPARKSEE_SESSION_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Session.h
 * \brief It contains the definition of Session class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/Handler.h"

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class Session;
}

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 * Forward declaration of sparksee classes.
 */
class Database;
class Graph;
class Objects;


/**
 * \brief Session class.
 *
 * A Session is a stateful period of activity of a user with the Database.

 * All the manipulation of a Database must be enclosed into a Session.
 * A Session can be initiated from a Database instance and allows for
 * getting a Graph instance which represents the persistent graph 
 * (the graph database).
 *
 * Also, temporary data is associated to the Session, thus when a Session
 * is closed, all the temporary data associated to the Session
 * is removed too. Objects or Values instances or even session attributes 
 * are an example of temporary data.
 *
 * Moreover, a Session is exclusive for a thread, thus if it is shared
 * among threads results may be fatal or unexpected.
 *
 * Check out the 'Processing' and 'Transactions' sections in the 
 * SPARKSEE User Manual for details about how Sessions work and the use of
 * transactions.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Session
: private Handler<sparksee_core::Session>
{
    friend class Database; // to access private constructor
    friend class Graph; // to access sparksee_core handled reference
    friend class Objects; // to access sparksee_core handled reference
    friend class ObjectsIterator;  // to access sparksee_core handled reference
    friend class Values; // to access sparksee_core handled reference
    friend class ValuesIterator; // to access sparksee_core handled reference
    friend class TextStream; // to access sparksee_core handled reference
    
public:
    
    /**
     * \brief Destructor.
     */
    virtual ~Session();
    
    /**
     * \brief Gets the Graph instance.
     * \return The Graph instance.
     */
    Graph * GetGraph();
    
    /**
     * \brief Creates a new Objects instance.
     * \return The new Objects instance.
     */
    Objects * NewObjects();
    
    /**
     * \brief Begins a transaction.
     */
    void Begin();
    
    /**
     * \brief Commits a transaction.
     */
    void Commit();
    
    
private:
    
    /**
     * \brief Creates a new instance.
     * \param core_sess [in] Handled sparksee_core Session reference.
     * \param db [in] Parent Database reference.
     */
    Session(sparksee_core::Session * core_sess, Database * db);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Session(const Session & session);

    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    Session & operator =(const Session & session);
    
    /**
     * \brief Parent Database reference.
     */
    Database * db;
};

END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_SESSION_H_
