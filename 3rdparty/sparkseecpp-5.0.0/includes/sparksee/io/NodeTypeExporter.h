/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_NODETYPEEXPORTER_H_
#define _SPARKSEE_NODETYPEEXPORTER_H_

/**
 * \addtogroup io
 * @{
 *
 * \file NodeTypeExporter.h
 * \brief It contains the definition of NodeTypeExporter class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "io/TypeExporter.h"


BEGIN_SPARKSEE_IO_NAMESPACE


/**
 * \brief NodeTypeExporter class.
 *
 * Specific TypeExporter implementation for node types.
 *
 * Check out the 'Data export' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT NodeTypeExporter
: public TypeExporter
{
public:
    
    /**
     * \brief Creates a new instance.
     */
    NodeTypeExporter()
    : TypeExporter()
    {
    }

    /**
     * \brief Creates a new instance.
     * \param rw [in] Output RowWriter.
     * \param g [in] Graph.
     * \param t [in] Type identifier.
     * \param attrs [in] Attribute identifiers to be exported.
     */
    NodeTypeExporter(RowWriter & rw, sparksee::gdb::Graph & g, sparksee::gdb::type_t t,
                     sparksee::gdb::AttributeList & attrs)
    : TypeExporter(rw, g, t, attrs)
    {
    }

    /**
     * \brief Destructor.
     */
    virtual ~NodeTypeExporter()
    {
    }

    /**
     * \brief See the TypeExporter class Run method
     */
    void Run() throw(sparksee::gdb::IOException, sparksee::gdb::Error)
    {
        sparksee::io::TypeExporter::RunProcess();
    }

};


END_SPARKSEE_IO_NAMESPACE

/** @} */
#endif // _SPARKSEE_NODETYPEEXPORTER_H_
