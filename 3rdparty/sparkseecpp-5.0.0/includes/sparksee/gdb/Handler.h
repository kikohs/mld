/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_HANDLER_H_
#define _SPARKSEE_HANDLER_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Handler.h
 * \brief It contains the definition of Handler class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/common.h"
#include "gdb/Exception.h"

BEGIN_SPARKSEE_GDB_NAMESPACE


/**
 * \brief Handles a reference.
 *
 * The handled reference is automatically destroyed (deleted) when the instance
 * is destroyed.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
template <typename T>
class SPARKSEE_EXPORT Handler
{
    
public:
    
    /**
     * \brief Creates a new instance.
     */
    Handler()
    : handler((T *)NULL)
    {
    }
    
    /**
     * \brief Creates a new instance with the given reference.
     * \param h [in] Reference to be handled.
     */
    Handler(T * h)
    : handler(h)
    {
    }
    
    /**
     * \brief Destructor
     * 
     * Frees the handled reference.
     */
    virtual ~Handler()
    {
        FreeHandler();
    }
    
    /**
     * \brief Gets the handled reference.
     * \return The handled reference.
     */
    T* GetHandler()
    {
        return handler;
    }
    
    /**
     * \brief Gets the handled reference.
     * \return The handled reference.
     */
    const T * GetHandler() const
    {
        return handler;
    }
    
protected:
    
    /**
     * \brief Sets the handled reference.
     * \param h [in] The handled reference.
     */
    void SetHandler(T * h)
    {
        handler = h;
    }
    
    /**
     * \brief Frees (deletes) the handled reference.
     */
    void FreeHandler();
    
    /**
     * \brief Gets if the handler is NULL.
     * \return TRUE if the handler is NULL, FALSE otherwise.
     */
    bool_t IsNull() const
    {
        return ((T*)NULL == handler);
    }
    
private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Handler(const Handler & handler);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    Handler & operator =(const Handler & handler);
    
    /**
     * \brief The handled reference.
     */
    T * handler;
};


END_SPARKSEE_GDB_NAMESPACE

#endif // _SPARKSEE_HANDLER_H_
