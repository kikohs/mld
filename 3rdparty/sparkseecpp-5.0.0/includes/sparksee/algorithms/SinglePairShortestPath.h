/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_SINGLEPAIRSHORTESTPATH_H_
#define _SPARKSEE_SINGLEPAIRSHORTESTPATH_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file SinglePairShortestPath.h
 * \brief It contains the definition of SinglePairShortestPath class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "algorithms/ShortestPath.h"


/*
 * Forward declaration of sparksee classes.
 */
namespace sparksee
{
    namespace gdb
    {
        class OIDList;
    }
}


BEGIN_SPARKSEE_ALGORITHMS_NAMESPACE


/**
 * \brief SinglePairShortestPath class.
 * 
 * Classes implementing this abstract class solve the
 * shortest path problem in a graph from a given source node and to
 * a given destination node.
 *
 * Check out the 'Algorithms' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT SinglePairShortestPath 
: public ShortestPath
{
public:
    
    /** 
     * Gets the shortest path between the source node and the destination node
     * as an ordered set of nodes.
     * \return Ordered set of node identifiers.
     */
    virtual sparksee::gdb::OIDList * GetPathAsNodes() = 0;
    
    /** 
     * Gets the shortest path between the source node and the destination node
     * as an ordered set of edges.
     * \return Ordered set of edge identifiers.
     */
    virtual sparksee::gdb::OIDList * GetPathAsEdges() = 0;
    
    /** 
     * Gets the cost of the shortest path.
     *
     * The cost for unweighted algorithms is the number of hops of the
     * shortest path. For weighted algoritms, the cost is the sum of
     * the costs of the edges of the shortest path.
     * \return The cost of the shortest path.
     */
    virtual sparksee::gdb::double64_t GetCost() = 0;
    
    /**
     * Returns TRUE If a path exists or FALSE otherwise.
     */
    virtual sparksee::gdb::bool_t Exists();

    /**
     * \brief Destructor.
     */
    virtual ~SinglePairShortestPath();
    
protected:
    
    /**
     * \brief Creates a new instance.
     * \param s [in] Session to get the graph from and perform traversal.
     * \param src [in] Source node.
     * \param dst [dst] Destination node.
     */
    SinglePairShortestPath(sparksee::gdb::Session &s,
        sparksee::gdb::oid_t src, sparksee::gdb::oid_t dst);

    /**
     * \brief Source node.
     */
    sparksee::gdb::oid_t source;
    
    /**
     * \brief Destination node.
     */
    sparksee::gdb::oid_t destination;
    
    /**
     * \brief Ordered set of node identifiers representing the shortest path.
     */
    sparksee::gdb::OIDList * pathAsNodes;

    /**
     * \brief Ordered set of edge identifiers representing the shortest path.
     *
     * May be computed lazily when requested from the pathAsNodes.
     */
    sparksee::gdb::OIDList * pathAsEdges;


private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    SinglePairShortestPath(const SinglePairShortestPath & trav);
    /**
     * \brief Non implemented to avoid use of empty constructor.
     */
    SinglePairShortestPath();    
};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_SINGLEPAIRSHORTESTPATH_H_
