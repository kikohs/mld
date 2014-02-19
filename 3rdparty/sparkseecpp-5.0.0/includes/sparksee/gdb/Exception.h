/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_EXCEPTION_H_
#define _SPARKSEE_EXCEPTION_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Exception.h
 * \brief It contains a hierarchy of exceptions.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/common.h"

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class SQLError;
}

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 *
 * EXCEPTIONS
 *
 */


/**
 * \brief Exception class.
 *
 * This is the superclass of those exceptions that can be thrown during
 * the normal execution of a program.
 *
 * It is expected all methods declare all Exception (or subclasses)  
 * they throw.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Exception
{
protected:
    
    /**
     * \brief Message of the exception.
     */
    std::string message;
    
public:
    
    /**
     * \brief Creates a new instance.
     */
    Exception();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    Exception(const std::string & mess);
    
    /**
     * \brief Gets the message of the exception.
	 *
	 * It should be called GetMessage but this is not possible
	 * because of a macro defined in Windows.
     * \return The message of the exception.
     */
    const std::string & Message() const;
    
    /**
     * \brief Sets the message of the exception.
     * \param mess [in] Message.
     */
    void SetMessage(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~Exception();
};


/**
 * \brief IO exception class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT IOException
: public Exception
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    IOException();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    IOException(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~IOException();
};


/**
 * \brief File not found exception class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT FileNotFoundException
: public IOException
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    FileNotFoundException();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    FileNotFoundException(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~FileNotFoundException();
};


/**
 * \brief No such element exception class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT NoSuchElementException
: public Exception
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    NoSuchElementException();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    NoSuchElementException(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~NoSuchElementException();
};


/**
 * \brief SQL exception class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT SQLException
: public Exception
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    SQLException();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    SQLException(const std::string & mess);
    
    /**
     * \brief Creates a new instance.
     * \param core_error [in] Sparkseecore SQLError.
     */
    SQLException(const sparksee_core::SQLError * core_error);
    
    /**
     * \brief Destructor.
     */
    virtual ~SQLException();
};


/*
 *
 * ERRORS
 *
 */


/**
 * \brief Error class.
 *
 * An Error corresponds to an unexpected and unpredictable exception.
 * 
 * As all methods can throw an Error at any moment, it is not expected 
 * they declare the Error (or subclasses) they may throw.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Error
: public Exception
{
    
public:
    
    /**
     * \brief Creates a new Error instance from a sparksee_core error code.
     * \param coreErrorCode [in] Sparkseecore error code.
     * \return Depending on the given sparksee_core error, this may return an 
     * Error instance or an specific Error subclass instance.
     */
    static Error NewError(int32_t coreErrorCode);
    
    /**
     * \brief Creates a new instance.
     */
    Error();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    Error(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~Error();
};


/**
 * \brief System error class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT SystemError
: public Error
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    SystemError();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    SystemError(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~SystemError();
};


/**
 * \brief Application error class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT AppError
: public Error
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    AppError();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    AppError(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~AppError();
};


/**
 * \brief Wrong argument error class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT WrongArgumentError
: public AppError
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    WrongArgumentError();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    WrongArgumentError(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~WrongArgumentError();
};


/**
 * \brief IO error class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT IOError
: public SystemError
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    IOError();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    IOError(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~IOError();
};


/**
 * \brief License error class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT LicenseError
: public AppError
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    LicenseError();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    LicenseError(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~LicenseError();
};


/**
 * \brief Unsupported operation error class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT UnsupportedOperationError
: public AppError
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    UnsupportedOperationError();
    
    /**
     * \brief Creates a new instance.
     * \param mess [in] Message of the exception.
     */
    UnsupportedOperationError(const std::string & mess);
    
    /**
     * \brief Destructor.
     */
    virtual ~UnsupportedOperationError();
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_EXCEPTION_H_
