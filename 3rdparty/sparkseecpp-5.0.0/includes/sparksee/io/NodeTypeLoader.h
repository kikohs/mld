/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_NODETYPELOADER_H_
#define _SPARKSEE_NODETYPELOADER_H_

/**
 * \addtogroup io
 * @{
 *
 * \file NodeTypeLoader.h
 * \brief It contains the definition of NodeTypeLoader class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "io/TypeLoader.h"


BEGIN_SPARKSEE_IO_NAMESPACE


/**
 * \brief NodeTypeLoader class.
 *
 * Specific TypeLoader implementation for node types.
 *
 * Check out the 'Data import' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT NodeTypeLoader
: public TypeLoader
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    NodeTypeLoader()
    : TypeLoader()
    {
    }


    /**
     * \brief Creates a new instance.
     * \param rr [in] Input RowReader.
     * \param g [in] Graph.
     * \param t [in] Type identifier.
     * \param attrs [in] Attribute identifiers to be loaded.
     * \param attrsPos [in] Attribute positions (column index >=0).
     */
    NodeTypeLoader(RowReader & rr, sparksee::gdb::Graph & g, sparksee::gdb::type_t t,
                   sparksee::gdb::AttributeList &attrs,
                   sparksee::gdb::Int32List &attrsPos)
    : TypeLoader(rr, g, t, attrs, attrsPos)
    {
    }

    /**
     * \brief Destructor.
     */
    virtual ~NodeTypeLoader()
    {
    }

    /**
     * \brief See the TypeLoader class Run method
     */
    void Run()
    throw(sparksee::gdb::IOException, sparksee::gdb::Error)
    {
        sparksee::io::TypeLoader::Run(ONE_PHASE, 1);
    }


    /**
     * \brief See the TypeLoader class RunTwoPhases method
     */
    void RunTwoPhases()
    throw(sparksee::gdb::IOException, sparksee::gdb::Error)
    {
        sparksee::io::TypeLoader::Run(TWO_PHASES, 1);
    }


    /**
     * \brief See the TypeLoader class RunNPhases method
     */
    void RunNPhases(sparksee::gdb::int32_t partitions)
    throw(sparksee::gdb::IOException, sparksee::gdb::Error)
    {
        sparksee::io::TypeLoader::Run(N_PHASES, partitions);
    }
};


END_SPARKSEE_IO_NAMESPACE

/** @} */
#endif // _SPARKSEE_NODETYPELOADER_H_
