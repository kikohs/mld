/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_VALUESITERATOR_H_
#define _SPARKSEE_VALUESITERATOR_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file ValuesIterator.h
 * \brief It contains the definition of ValuesIterator class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/Handler.h"
#include "gdb/Value.h"

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
class Values;

/**
 * \brief Values iterator class.
 *
 * It allows for traversing all the elements into a Values instance.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT ValuesIterator
: private Handler<sparksee_core::Values>
{
    friend class Values; // to access private constructor
    
public:
    
    /**
     * \brief Destructor.
     */
    virtual ~ValuesIterator();
    
    /**
     * \brief Gets if there are more elements to traverse.
     * \return TRUE if there are more elements to traverse,
     * FALSE otherwise.
     */
    bool_t HasNext();
    
    /**
     * \brief Gets the next element to traverse.
     * \return The next element.
     */
    Value* Next();

private:
    
    /**
     * \brief Creates a new instance.
     * \param core_values [in] Sparkseecore Values reference.
     * \param vv [in] Parent Values reference.
     */
    ValuesIterator(sparksee_core::Values * core_values, Values * vv);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    ValuesIterator(const ValuesIterator & objs);

    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    ValuesIterator & operator =(const ValuesIterator & objs);
    
    /**
     * \brief Parent Values reference.
     */
    Values * values;
    
    /**
     * \brief Invalidates the ValuesIterator and deletes the sparksee_core instance.
     * It's used when the parent Values is deleted while it still has ValuesIterator
     * open.
     */
    void Invalidate();
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_VALUESITERATOR_H_
