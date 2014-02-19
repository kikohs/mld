/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_SINGLEPAIRSHORTESTPATHBFS_H_
#define _SPARKSEE_SINGLEPAIRSHORTESTPATHBFS_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file SinglePairShortestPathBFS.h
 * \brief It contains the definition of SinglePairShortestPathBFS class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "algorithms/SinglePairShortestPath.h"
#include "gdb/Value.h"

BEGIN_SPARKSEE_ALGORITHMS_NAMESPACE


/**
 * \brief SinglePairShortestPathBFS class.
 * 
 * It solves the single-pair shortest path problem using a BFS-based
 * implementation.
 * 
 * It is a unweighted algorithm, that is it assumes all edges have the same
 * cost.
 *
 * Check out the 'Algorithms' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT SinglePairShortestPathBFS 
: public SinglePairShortestPath
{
public:
    
    /**
     * \brief Destructor.
     */
    virtual ~SinglePairShortestPathBFS();
    
    /**
     * \brief Executes the algorithm.
     */
    virtual void Run();
    
    /**
     * Gets the shortest path between the source node and the destination node
     * as an ordered set of nodes.
     * \return Ordered set of node identifiers.
     */
    virtual sparksee::gdb::OIDList * GetPathAsNodes();
    
    /**
     * Gets the shortest path between the source node and the destination node
     * as an ordered set of edges.
     * \return Ordered set of edge identifiers.
     */
    virtual sparksee::gdb::OIDList * GetPathAsEdges();
    
    /**
     * Gets the cost of the shortest path.
     *
     * The cost is the number of hops of the shortest path.
     * \return The cost of the shortest path.
     */
    virtual sparksee::gdb::double64_t GetCost();
    
    /**
     * \brief Creates a new instance.
     * \param s [in] Session to get the graph from and perform traversal.
     * \param src [in] Source node.
     * \param dst [dst] Destination node.
     */
    SinglePairShortestPathBFS(sparksee::gdb::Session &s,
        sparksee::gdb::oid_t src, sparksee::gdb::oid_t dst);

    /**
     * \brief Set that only the path existence must be calculated and not the path itself.
     * That method should improve the performance of the algorithm, but a
     * call to GetPathAsNodes or GetPathAsEdges will generate an exception even if the path exists.
     */
    virtual void CheckOnlyExistence();

private:
    /**
     * \brief Check only the existence (true) or keep the path found (false).
     */
    sparksee::gdb::bool_t m_checkOnlyExistence;
    /**
     * \brief The number of hops in the path found.
     */
    sparksee::gdb::double64_t m_cost;

    /**
     * \brief Attribute to keep the predecessor of each visited node.
     * It's only used when more than MAX_MAPPED_PREDECESSORS nodes have been visited.
     */
    sparksee::gdb::attr_t m_predecessorAttr;
    /**
     * \brief Predecessors map type definition.
     */
    typedef std::map<sparksee::gdb::oid_t, sparksee::gdb::oid_t> PredecessorsMap;
    /**
     * \brief A map to keep the predecessor of each visited node.
     */
    PredecessorsMap m_predecessors;
    /**
     * \brief Maximum number of nodes to be kept using only the map.
     */
    static const sparksee::gdb::int32_t MAX_MAPPED_PREDECESSORS;
    /**
     * \brief Temporary Value for getting/setting the predecessor attribute.
     */
    sparksee::gdb::Value m_predecessorValue;

    /**
     * \brief Sets the predecessor of a given node.
     * It keeps the predecessor of the given node in the map or as an attribute.
     * @param node [in] The child node that will store the information.
     * @param predecessor [in] The oid that will be kept as the node parent.
     */
    void SetPredecessor(sparksee::gdb::oid_t node, sparksee::gdb::oid_t predecessor);

    /**
     * \brief Gets the predecessor of the given node.
     * It will use the map and the attribute to find the predecessor.
     * @param node [in] The node oid.
     * @return Returns the predecessor of the given node.
     *
     */
    sparksee::gdb::oid_t GetPredecessor(sparksee::gdb::oid_t node);


    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    SinglePairShortestPathBFS(const SinglePairShortestPathBFS & trav);
    /**
     * \brief Non implemented to avoid use of empty constructor.
     */
    SinglePairShortestPathBFS();    

    /**
     * Find an edge from the given set that had not been excluded.
     * @return Returns the edge identifier or InvalidOID.
     */
    sparksee::gdb::oid_t FindAValidEdge(sparksee::gdb::Objects *edges);
    /**
     * Find an edge from the given edge type and direction between
     * the nodes specified that had not been excluded.
     * @return Returns the edge identifier or InvalidOID.
     */
    sparksee::gdb::oid_t GetAValidEdge( sparksee::gdb::type_t edgeT,
            sparksee::gdb::EdgesDirection dir, sparksee::gdb::oid_t predNode,
            sparksee::gdb::oid_t current);

};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_SINGLEPAIRSHORTESTPATHBFS_H_
