/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_WEAKCONNECTIVITY_H_
#define _SPARKSEE_WEAKCONNECTIVITY_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file WeakConnectivity.h
 * \brief It contains the definition of WeakConnectivity class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "algorithms/Connectivity.h"


BEGIN_SPARKSEE_ALGORITHMS_NAMESPACE

/**
 * \brief WeakConnectivity class.
 * 
 * Any class implementing this abstract class can be used to solve the problem
 * of finding weakly connected components in an <b> undirected </b> graph or in
 * a directed graph which will be considered as an undirected one.
 *
 * It consists in finding components where every pair (u,v)
 * of nodes contained in it has a path from u to v and
 * from v to u.
 *
 * It is possible to set some restrictions after constructing a new instance
 * of this class and before running it in order to limit the results.
 *
 * After the execution, we can retrieve the results stored in an instance of the
 * ConnectedComponents class using the getConnectedComponents method.
 *
 * Check out the 'Algorithms' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT WeakConnectivity : public Connectivity
{
public:

    /**
     * \brief Destructor.
     */
    virtual ~WeakConnectivity();

    /**
     * \brief Allows connectivity through edges of the given type.
     * In a weak connectivity the edges can be used in {@link #Any} direction.
     * \param t [in] Edge type.
     * \param d [in] Edge direction.
     */
    virtual void AddEdgeType(sparksee::gdb::type_t t);

    /**
     * \brief Allows connectivity through all edge types of the graph.
     * In a weak connectivity the edges can be used in {@link #Any} direction.
     * \param d [in] Edge direction.
     */
    virtual void AddAllEdgeTypes();


protected:

    /**
     * Creates a new instance of {@link WeakConnectivity}.
     *
     * \param s [in] Session to get the graph from and calculate the connectivity
     */
    WeakConnectivity(sparksee::gdb::Session &s);

private:

    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    WeakConnectivity(const WeakConnectivity & wc);
    /**
     * \brief Non implemented to avoid use of empty constructor.
     */
    WeakConnectivity();
};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_WEAKCONNECTIVITY_H_
