/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_SHORTESTPATH_H_
#define _SPARKSEE_SHORTESTPATH_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file ShortestPath.h
 * \brief It contains the definition of ShortestPath class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "gdb/common.h"
#include <map>


/*
 * Forward declaration of sparksee classes.
 */
namespace sparksee
{
    namespace gdb
    {
        class Session;
        class Graph;
        class Objects;
    }
}


BEGIN_SPARKSEE_ALGORITHMS_NAMESPACE


/**
 * \brief ShortestPath class.
 * 
 * Classes implementing this abstract class solve the
 * shortest path problem in a graph.
 *
 * The user must set which node and edge types can be used for the
 * traversal. 
 *
 * Check out the 'Algorithms' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT ShortestPath
{
public:
    
    /** 
     * \brief Sets the maximum hops restriction.
     *
     * All paths longer than the maximum hops restriction will be ignored.
     * \param m [in] The maximum  hops restriction. 
     * It must be positive or zero. Zero, the default value, means unlimited.
     */
    void SetMaximumHops(sparksee::gdb::int32_t maxhops);
    
    /**
     * \brief Allows for traversing edges of the given type.
     * \param t [in] Edge type.
     * \param d [in] Edge direction.
     */
    virtual void AddEdgeType(sparksee::gdb::type_t t, sparksee::gdb::EdgesDirection d);
    
    /**
     * \brief Allows for traversing all edge types of the graph.
     * \param d [in] Edge direction.
     */
    virtual void AddAllEdgeTypes(sparksee::gdb::EdgesDirection d);

    /**
     * \brief Allows for traversing nodes of the given type.
     */
    virtual void AddNodeType(sparksee::gdb::type_t t);

    /**
     * \brief Allows for traversing all node types of the graph.
     */
    virtual void AddAllNodeTypes();

    /**
     * \brief Set which nodes can't be used.
     * This will replace any previously specified set of excluded nodes.
     * Should only be used to exclude the usage of specific nodes
     * from allowed node types because it's less efficient than
     * not allowing a node type.
     * \param nodes [in] A set of node identifiers that must be kept intact
     * until the destruction of the class.
     */
    virtual void ExcludeNodes(sparksee::gdb::Objects &nodes);

    /**
     * \brief Set which edges can't be used.
     * This will replace any previously specified set of excluded edges.
     * Should only be used to exclude the usage of specific edges
     * from allowed edge types because it's less efficient than
     * not allowing an edge type.
     * \param edges [in] A set of edge identifiers that must be kept intact
     * until the destruction of the class.
     */
    virtual void ExcludeEdges(sparksee::gdb::Objects &edges);

    /** 
     * \brief Runs the algorithm.
     * 
     * This method can only be called once.
     */
    virtual void Run() = 0;
    
    /**
     * \brief Destructor.
     */
    virtual ~ShortestPath();
    
protected:
    
    /**
     * \brief Creates a new instance.
     * \param s [in] Session to get the graph from and perform traversal.
     */
    ShortestPath(sparksee::gdb::Session &s);
    
    /**
     * \brief Session.
     */
    sparksee::gdb::Session * sess;
    /**
     * \brief Graph.
     */
    sparksee::gdb::Graph * graph;
    /**
     * \brief Allowed edge types.
     */
    std::map<sparksee::gdb::type_t, sparksee::gdb::EdgesDirection> edgeTypes;
    /**
     * \brief Allowed node types.
     */
    std::vector<sparksee::gdb::type_t> nodeTypes;
    /**
     * \brief Maximum hops restriction.
     * 
     * It must be positive or zero. Zero means unlimited.
     */
    sparksee::gdb::int32_t maxHops;
    /**
     * \brief True if the shortest path has been calculated.
     */
    sparksee::gdb::bool_t computed;
    /**
     * \brief The set of excluded nodes.
     */
    sparksee::gdb::Objects *excludedNodes;
    /**
     * \brief The set of excluded edges.
     */
    sparksee::gdb::Objects *excludedEdges;
    
    /**
     * \brief True if all the node types are allowed.
     */
    sparksee::gdb::bool_t areAllNodeTypesAllowed;

    /**
     * Check that the given edge type is valid
     */
    void AssertEdgeType(sparksee::gdb::type_t edgetype);
    /**
     * Check that edges had been added.
     */
    void AssertAddedEdges();
    /**
     * Check that nodes had been added.
     */
    void AssertAddedNodes();
    /**
     * Check that the given node type is valid
     */
    void AssertNodeType(sparksee::gdb::type_t nodetype);
    /**
     * Check if the given node has an allowed type.
     */
    sparksee::gdb::bool_t IsNodeTypeAllowed(sparksee::gdb::oid_t nodeId);
    /**
     * Check that the shortest path had not been calculated yet.
     */
    void AssertNotComputed();
    /**
     * Check that the shortest path had been calculated.
     */
    void AssertComputed();
    /**
     * Check if the given node is forbidden.
     */
    sparksee::gdb::bool_t IsNodeExcluded(sparksee::gdb::oid_t node);
    /**
     * Check if the given edge is forbidden.
     */
    sparksee::gdb::bool_t IsEdgeExcluded(sparksee::gdb::oid_t edge);

private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    ShortestPath(const ShortestPath & trav);
    /**
     * \brief Non implemented to avoid use of empty constructor.
     */
    ShortestPath();    
};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_SHORTESTPATH_H_
