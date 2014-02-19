/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_CONTEXT_H_
#define _SPARKSEE_CONTEXT_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file Context.h
 * \brief It contains the definition of Context class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "gdb/common.h"
#include <map>
#include <vector>
#include <algorithm>

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
        class TypeList;
    }
}


BEGIN_SPARKSEE_ALGORITHMS_NAMESPACE


/**
 * \brief Context class.
 * 
 * It provides a very similar functionality than the Traversal classes. The
 * main difference is Context returns a resulting collection whereas
 * Traversal provides an iterator behaviour.
 *
 * Check out the 'Algorithms' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Context
{
public:
    
    /**
     * \brief Allows for traversing edges of the given type.
     * \param t [in] Edge type.
     * \param d [in] Edge direction.
     */
    void AddEdgeType(sparksee::gdb::type_t t, sparksee::gdb::EdgesDirection d);
    
    /**
     * \brief Allows for traversing all edge types of the graph.
     * \param d [in] Edge direction.
     */
    void AddAllEdgeTypes(sparksee::gdb::EdgesDirection d);
    
    /**
     * \brief Allows for traversing nodes of the given type.
     */
    void AddNodeType(sparksee::gdb::type_t t);
    
    /**
     * \brief Allows for traversing all node types of the graph.
     */
    void AddAllNodeTypes();
    
    /**
     * \brief Set which nodes can't be used.
     * This will replace any previously specified set of excluded nodes.
     * Should only be used to exclude the usage of specific nodes
     * from allowed node types because it's less efficient than
     * not allowing a node type.
     * \param nodes [in] A set of node identifiers that must be kept intact
     * until the destruction of the class.
     */
    void ExcludeNodes(sparksee::gdb::Objects &nodes);

    /**
     * \brief Set which edges can't be used.
     * This will replace any previously specified set of excluded edges.
     * Should only be used to exclude the usage of specific edges
     * from allowed edge types because it's less efficient than
     * not allowing an edge type.
     * \param edges [in] A set of edge identifiers that must be kept intact
     * until the destruction of the class.
     */
    void ExcludeEdges(sparksee::gdb::Objects &edges);

    /**
     * \brief Gets the resulting collection of nodes.
     * \return The resulting collection of nodes.
     */
    sparksee::gdb::Objects * Compute();
    
    /**
     * \brief Sets the maximum hops restriction.
     *
     * All paths longer than the maximum hops restriction will be ignored.
     * \param maxhops [in] The maximum  hops restriction.
     * It must be positive or zero. Zero, the default value, means unlimited.
     * \param include [in] If TRUE, the resulting collection will include
     * those nodes at distance less or equal than the given one, otherwise it
     * will just include those nodes at distance equal than the given one. 
     * This parameter just makes sense if maxhops is different from 0; in that
     * case it includes all nodes no matters the distance.
     */
    void SetMaximumHops(sparksee::gdb::int32_t maxhops, sparksee::gdb::bool_t include);

    /**
     * \brief Destructor.
     */
    virtual ~Context();
    
    /**
     * \brief Creates a new instance.
     * \param s [in] Session to get the graph from and perform operation.
     * \param node [in] Node to start traversal from.
     */
    Context(sparksee::gdb::Session &s, sparksee::gdb::oid_t node);
    
    /**
     * \brief Helper method to easily compute a context from a node.
     * \param s [in] Session to get the graph from and perform operation.
     * \param node [in] Node to start traversal from.
     * \param nodeTypes [in] Allowed node type list. NULL means all node types are allowed.
     * \param edgeTypes [in] Allowed edge type list. NULL means all edge types are allowed.
     * \param dir [in] Allowed direction for the allowed edge types.
     * \param maxhops [in] The maximum  hops restriction.
     * It must be positive or zero. Zero, the default value, means unlimited.
     * \param include [in] If TRUE, the resulting collection will include
     * those nodes at distance less or equal than the given one, otherwise it
     * will just include those nodes at distance equal than the given one. 
     * This parameter just makes sense if maxhops is different from 0; in that
     * case it includes all nodes no matters the distance.
     */
    static sparksee::gdb::Objects * Compute(sparksee::gdb::Session & s, sparksee::gdb::oid_t node,
                                       sparksee::gdb::TypeList * nodeTypes, 
                                       sparksee::gdb::TypeList * edgeTypes, 
                                       sparksee::gdb::EdgesDirection dir, 
                                       sparksee::gdb::int32_t maxhops,
                                       sparksee::gdb::bool_t include);
    
protected:
    
    /**
     * \brief Session.
     */
    sparksee::gdb::Session * sess;
    /**
     * \brief Graph.
     */
    sparksee::gdb::Graph * graph;
    /**
     * \brief Source node of the traversal.
     */
    sparksee::gdb::oid_t src;
    /**
     * \brief Allowed edge types.
     */
    std::map<sparksee::gdb::type_t, sparksee::gdb::EdgesDirection> edgeTypes;
    /**
     * \brief Allowed node types.
     */
    std::vector<sparksee::gdb::type_t> nodeTypes;
    /**
     * \brief Maximum number of hops allowed.
     */
    sparksee::gdb::int32_t maxHops;
    /**
     * \brief Include those nodes at distance <= maxhops or just those
     * nodes at distance == maxhops.
     */
    sparksee::gdb::bool_t inclusive;
    /**
     * \brief The set of excluded nodes.
     */
    sparksee::gdb::Objects *excludedNodes;
    /**
     * \brief The set of excluded edges.
     */
    sparksee::gdb::Objects *excludedEdges;

private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Context(const Context & cntxt);
    /**
     * \brief Non implemented to avoid use of empty constructor.
     */
    Context();    

    /**
     * Check that the given edge type is valid
     */
    void AssertEdgeType(sparksee::gdb::type_t edgetype);
    /**
     * Check that the given node type is valid
     */
    void AssertNodeType(sparksee::gdb::type_t nodetype);
};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_CONTEXT_H_
