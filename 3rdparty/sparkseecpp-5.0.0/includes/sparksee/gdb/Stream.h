/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_STREAM_H_
#define _SPARKSEE_STREAM_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Stream.h
 * \brief It contains the definition of stream classes.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/Handler.h"

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class GraphTextStream;
}

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 * Forward declaration of sparksee classes.
 */
class Value;
class Session;


/**
 * \brief TextStream class.
 *
 * It allows for reading and writting Text attribute values.
 *
 * It is very important to close the stream once no more reading or writting
 * operations will be performed to ensure data is successfully stored.
 *
 * Whereas string attributes are set and got using the Value class, text
 * attributes are operated using a stream pattern.
 *
 * Use of TextStream for writing: (i) Create a TextStream instance and
 * (ii) set the stream for a text attribute of a node or edge instance
 * with the graph SetAttributeText method. Once the set attribute text
 * has been done, (iii) perform as many write operations as you need to
 * the TextStream instance. Lastly, (iv) exeucte Close to flush and
 * close the stream.
 *
 * Use of TextStream for reading: (i) Get the stream of a text attribute
 * of a node or edge instance with the GetAttributeText graph method.
 * Once you have the TextStream instance, (ii) you can execute Read operations
 * to read from the stream. (iii) The end of the stream
 * is reached when Read returns 0. Finally, (iv) execute Close to close
 * stream resources.
 *
 * Check out the 'Attributes and values' section in the SPARKSEE User Manual 
 * for more details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT TextStream
: private Handler<sparksee_core::GraphTextStream>
{
    friend class Graph; // to access private constructor and private handler
    
    /*
     * DOCUMENTATION FOR CODERS
     * 
     * Read and Write methods have an specific name for the parameters
     * because is required for SWIG to specify the behaviour of the argument.
     */
public:
    
    /**
     * \brief Creates a new instance.
     *
     * A TextStream only can be created by the user to write data.
     * \param append [in] If TRUE, the it is created in append mode to 
     * write from the end of the stream, otherwise it is created to
     * write from the begining of the stream.
     */
    TextStream(bool_t append);
    
    /**
     * \brief Read data from the stream.
     * \param dataOUT [out] Buffer to read data to. It must be allocated by
     * the user.
     * \param length [in] Length of the given data buffer. It must be > 0.
     * \return Amount of read data (<= length). If 0, there is no more
     * data to be read from the stream.
     */
    int32_t Read(uchar_t * dataOUT, int32_t length) const;
    
    /**
     * \brief Write data to the stream.
     * \param dataIN [in] Buffer to write data from.
     * \param length [in] Length of the data buffer. It must be > 0.
     */
    void Write(const uchar_t * dataIN, int32_t length);
    
    /**
     * \brief Closes the stream.
     * 
     * Once the Stream is closed, it cannot be used again.
     */
    void Close();
    
    /**
     * \brief Destructor.
     */
    virtual ~TextStream();
    
    /**
     * \brief Returns TRUE if the stream is not available.
     *  It returns FALSE if the stream is ready for reading or writing data.
	 * \return FALSE if the stream is ready
	 */
    bool_t IsNull() const;

private:
    
    /**
     * \brief Creates a new instance.
     * \param core_tstrm [in] Handled sparksee_core TextStream reference.
     */
    TextStream(sparksee_core::GraphTextStream *core_tstrm);
    TextStream(sparksee_core::GraphTextStream *core_tstrm, Session *s);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    TextStream(const TextStream &tstrm);

    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    TextStream & operator =(const TextStream &tstrm);

    Session* sess;
    void SetSession(Session *s);
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_STREAM_H_
