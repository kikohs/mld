/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_ROWREADER_H_
#define _SPARKSEE_ROWREADER_H_

/**
 * \addtogroup io
 * @{
 *
 * \file RowReader.h
 * \brief It contains the definition of RowReader interface.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/common.h"
#include "gdb/Exception.h"
#include "gdb/Graph_data.h"


BEGIN_SPARKSEE_IO_NAMESPACE


/**
 * \brief RowReader interface.
 *
 * Common interface for those readers which get the data as an string array.
 *
 * It works as follows: perform as many read operations as necessary and call
 * close once at the end. Once close is called no more read operations can
 * be executed.
 * 
 * Check out the 'Data import' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT RowReader
{
public:
    
    /**
     * \brief Moves the reader to the beginning.
     * <p>
     * Restarts the reader.
     * @return <code>true</code> if the reader can be restarted,
     * <code>false</code> otherwise.
     * \exception IOException If bad things happen during the restart.
     */
    virtual sparksee::gdb::bool_t Reset()
    throw(sparksee::gdb::IOException) = 0;

    /**
     * \brief Reads the next row as a string array.
     * \param row [out] A string list with each comma-separated element as
     * a separate entry.
     * \return Returns true if a row had been read or false otherwise.
     * \exception IOException If bad things happen during the read.
     */
    virtual sparksee::gdb::bool_t Read(sparksee::gdb::StringList &row)
    throw(sparksee::gdb::IOException) = 0;

    /**
     * \brief The row number for the current row.
     * @return The current row number; 0 if there is no current row.
     * \exception IOException If it fails.
     */
    virtual sparksee::gdb::int32_t GetRow()
    throw(sparksee::gdb::IOException) = 0;
    
    /**
     * \brief Closes the reader.
     * \exception IOException If the close fails.
     */
    virtual void Close()
    throw(sparksee::gdb::IOException) = 0;

    /**
     * \brief Destructor.
     */
    virtual ~RowReader()
    {
    }


private:

    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    RowReader(const RowReader & rd);

    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    RowReader & operator =(const RowReader & rd);

protected:

    /**
     * \brief Empty constructor.
     *
     * Protected because no one should instantiate a RowReader. Just inherited
     * classes can use this empty constructor.
     */
    RowReader()
    {
    }
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_ROWREADER_H_
