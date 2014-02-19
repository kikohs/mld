/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_EDGETYPEEXPORTER_H_
#define _SPARKSEE_EDGETYPEEXPORTER_H_

/**
 * \addtogroup io
 * @{
 *
 * \file EdgeTypeExporter.h
 * \brief It contains the definition of EdgeTypeExporter class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "io/TypeExporter.h"


BEGIN_SPARKSEE_IO_NAMESPACE


/**
 * \brief EdgeTypeExporter class.
 *
 * Specific TypeExporter implementation for edge types.
 *
 * Check out the 'Data export' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT EdgeTypeExporter
: public TypeExporter
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    EdgeTypeExporter()
    : TypeExporter()
    {
    }


    /**
     * \brief Creates a new instance.
     * \param rw [in] Output RowWriter.
     * \param g [in] Graph.
     * \param t [in] Type identifier.
     * \param attrs [in] Attribute identifiers to be exported.
     * \param hPos [in] The position (index column) for the head value.
     * \param tPos [in] The position (index column) for the tail value.
     * \param hAttr [in] The attribute identifier to get the value to be dumped 
     * for the head.
     * \param tAttr [in] The attribute identifier to get the value to be dumped 
     * for the tail.
     */
    EdgeTypeExporter(RowWriter & rw, sparksee::gdb::Graph & g, sparksee::gdb::type_t t,
                     sparksee::gdb::AttributeList & attrs, 
                     sparksee::gdb::int32_t hPos, sparksee::gdb::int32_t tPos,
                     sparksee::gdb::attr_t hAttr, sparksee::gdb::attr_t tAttr)
    : TypeExporter(rw, g, t, attrs)
    {
        SetHeadAttribute(hAttr);
        SetHeadPosition(hPos);
        SetTailAttribute(tAttr);
        SetTailPosition(tPos);
    }

    /**
     * \brief Destructor.
     */
    virtual ~EdgeTypeExporter()
    {
    }

    /**
     * \brief See the TypeExporter class Run method
     */
    void Run() throw(sparksee::gdb::IOException, sparksee::gdb::Error) 
    {
        sparksee::io::TypeExporter::RunProcess();
    }


    /**
     * \brief Sets the attribute that will be used to get the value to be dumped
     * for the head of the edge.
     * \param attr [in] Head Attribute
     */
    void SetHeadAttribute(sparksee::gdb::attr_t attr)
    {
        TypeExporter::SetHeadAttribute(attr);
    }

    /**
     * \brief Sets the position (index column) of the head attribute in the exported data.
     * \param pos [in] Head position
     */
    void SetHeadPosition(sparksee::gdb::int32_t pos)
    {
        TypeExporter::SetHeadPosition(pos);
    }

    /**
     * \brief Sets the attribute that will be used to get the value to be dumped
     * for the tail of the edge.
     * \param attr [in] Tail Attribute
     */
    void SetTailAttribute(sparksee::gdb::attr_t attr)
    {
        TypeExporter::SetTailAttribute(attr);
    }

    /**
     * \brief Sets the position (index column) of the tail attribute in the exported data.
     * \param pos [in] Tail position
     */
    void SetTailPosition(sparksee::gdb::int32_t pos)
    {
        TypeExporter::SetTailPosition(pos);
    }
};


END_SPARKSEE_IO_NAMESPACE

/** @} */
#endif // _SPARKSEE_EDGETYPEEXPORTER_H_
