/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_VALUES_H_
#define _SPARKSEE_VALUES_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Values.h
 * \brief It contains the definition of Values class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/Handler.h"
#include <list>

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class Values;
}

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 * Forward declaration of sparksee classes.
 */
class Session;
class ValuesIterator;
class Graph;


/**
 * \brief Value set class.
 *
 * This is a set of Value instances, that is there is no duplicated elements.
 *
 * Use a ValuesIterator to traverse all the elements into the set.
 *
 * When the Values instance is closed, it closes all existing and non-closed
 * ValuesIterator instances too.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Values
: private Handler<sparksee_core::Values>
{
    friend class Graph; // to access private constructor
    friend class ValuesIterator; // to unregister iterators
    
public:
    
    /**
     * \brief Destructor.
     */
    virtual ~Values();
    
    /**
     * \brief Gets the number of elements into the collection.
     * \return The number of elements into the collection.
     */
    int64_t Count();
    
    /**
     * \brief Gets a ValuesIterator.
     * \return ValuesIterator instance.
     * \param order [in] Ascending or descending order.
     */
    ValuesIterator * Iterator(Order order);

private:
    
    /**
     * \brief Creates a new instance.
     * \param core_values [in] Sparkseecore Values reference.
     * \param s [in] Session reference.
     * \param g [in] Graph reference.
     * \param a [in] Sparksee attribute identifier.
     */
    Values(sparksee_core::Values * core_values, Session * s, Graph * g, attr_t a);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Values(const Values & values);

    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    Values & operator =(const Values & values);
    
    /**
     * \brief Session reference.
     */
    Session * sess;

    /**
     * \brief Graph reference.
     */
    Graph * graph;
    
    /**
     * \brief Sparksee attribute identifier.
     */
    attr_t attr;
    
    /**
     * \brief List of open iterators.
     */
    std::list<ValuesIterator *> m_iterators;
    
    /**
     * \brief Unregister the iterator.
     * It's called when a ValuesIterator is deleted.
     */
    void UnregisterIterator(ValuesIterator *it);
    
    /**
     * \brief Invalidates all the open iterators.
     */
    void InvalidateIterators();
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_VALUES_H_
