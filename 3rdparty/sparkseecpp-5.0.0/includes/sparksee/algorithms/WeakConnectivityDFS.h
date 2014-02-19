/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_WEAKCONNECTIVITYDFS_H_
#define _SPARKSEE_WEAKCONNECTIVITYDFS_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file WeakConnectivityDFS.h
 * \brief It contains the definition of WeakConnectivityDFS class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "algorithms/WeakConnectivity.h"


BEGIN_SPARKSEE_ALGORITHMS_NAMESPACE


/**
 * \brief WeakConnectivityDFS class.
 * 
 * This class can be used to solve the problem of finding weakly connected
 * components in an <b> undirected </b> graph or in a directed graph which will
 * be considered as an undirected one.
 *
 * It consists in finding components where every pair (u,v)
 * of nodes contained in it has a path from u to v and
 * from v to u.  This implementation is based on the
 * Depth-First Search (DFS) algorithm.
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
class SPARKSEE_EXPORT WeakConnectivityDFS : public WeakConnectivity
{
public:

    /**
     * Creates a new instance of {@link WeakConnectivityDFS}.
     * After creating this instance is required to indicate the set of edge types
     * and the set of node types which will be navigated through while traversing
     * the graph in order to find the weak connected components.
     * \param s [in] Session to get the graph from and calculate the connectivity
     */
    WeakConnectivityDFS(sparksee::gdb::Session &s);

    /**
     * \brief Destructor.
     */
    virtual ~WeakConnectivityDFS();
    
    /**
     * \brief Executes the algorithm.
     */
    void Run();

private:

    /**
     * \brief Calculate the connected component of a node.
     */
    void ComputeDFS(sparksee::gdb::oid_t idNode);

};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_WEAKCONNECTIVITYDFS_H_
