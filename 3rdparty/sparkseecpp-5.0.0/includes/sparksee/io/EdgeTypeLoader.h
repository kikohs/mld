/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_EDGETYPELOADER_H_
#define _SPARKSEE_EDGETYPELOADER_H_

/**
 * \addtogroup io
 * @{
 *
 * \file EdgeTypeLoader.h
 * \brief It contains the definition of EdgeTypeLoader class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "io/TypeLoader.h"


BEGIN_SPARKSEE_IO_NAMESPACE


/**
 * \brief EdgeTypeLoader class.
 *
 * Specific TypeLoader implementation for edge types.
 *
 * Check out the 'Data import' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT EdgeTypeLoader
: public TypeLoader
{
public:
    /**
     * \brief Creates a new instance.
     */
    EdgeTypeLoader()
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
     * to all listeners.
     * \param hPos [in] The position (index column) for the head value.
     * \param tPos [in] The position (index column) for the tail value.
     * \param hAttr [in] The attribute identifier for the head.
     * \param tAttr [in] The attribute identifier for the tail.
     */
    EdgeTypeLoader(RowReader & rr, sparksee::gdb::Graph & g, sparksee::gdb::type_t t,
                   sparksee::gdb::AttributeList &attrs,
                   sparksee::gdb::Int32List &attrsPos,
                   sparksee::gdb::int32_t hPos, sparksee::gdb::int32_t tPos,
                   sparksee::gdb::attr_t hAttr, sparksee::gdb::attr_t tAttr)
    : TypeLoader(rr, g, t, attrs, attrsPos)
    {
        SetHeadAttribute(hAttr);
        SetHeadPosition(hPos);
        SetTailAttribute(tAttr);
        SetTailPosition(tPos);
    }

    /**
     * \brief Destructor.
     */
    virtual ~EdgeTypeLoader()
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


    /**
     * \brief Sets the attribute that will be used to find the head of the edge.
     * This method is protected because only the Edge loaders should have it.
     * \param attr [in] Head Attribute
     */
    void SetHeadAttribute(sparksee::gdb::attr_t attr)
    {
        TypeLoader::SetHeadAttribute(attr);
    }
    /**
     * \brief Sets the position of the head attribute in the source data.
     * This method is protected because only the Edge loaders should have it.
     * \param pos [in] Head position
     */
    void SetHeadPosition(sparksee::gdb::int32_t pos)
    {
        TypeLoader::SetHeadPosition(pos);
    }
    /**
     * \brief Sets the attribute that will be used to find the tail of the edge.
     * This method is protected because only the Edge loaders should have it.
     * \param attr [in] Tail Attribute
     */
    void SetTailAttribute(sparksee::gdb::attr_t attr)
    {
        TypeLoader::SetTailAttribute(attr);
    }
    /**
     * \brief Sets the position of the tail attribute in the source data.
     * This method is protected because only the Edge loaders should have it.
     * \param pos [in] Tail position
     */
    void SetTailPosition(sparksee::gdb::int32_t pos)
    {
        TypeLoader::SetTailPosition(pos);
    }

};


END_SPARKSEE_IO_NAMESPACE

/** @} */
#endif // _SPARKSEE_EDGETYPELOADER_H_
