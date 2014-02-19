/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_TRAVERSAL_H_
#define _SPARKSEE_TRAVERSAL_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file Traversal.h
 * \brief It contains the definition of Traversal class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "gdb/common.h"
#include <map>
#include <vector>


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
 * \brief Traversal class.
 * 
 * Any class implementing this abstract class can be used to traverse a graph.
 *
 * Once the instance has been created and the allowed node and edge types
 * has been set, it can be used as an iterator, retrieving the next object
 * identifier of the traversal until there are no more.
 *
 * Check out the 'Algorithms' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Traversal
{
public:
    
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
     * \brief Gets the next object of the traversal.
     * \return A node or edge identifier.
     */
    virtual sparksee::gdb::oid_t Next() = 0;
    
    /**
     * \brief Gets if there are more objects to be traversed.
     * \return TRUE if there are more objects, FALSE otherwise.
     */
    virtual sparksee::gdb::bool_t HasNext() = 0;
    
    /**
     * \brief Returns the depth of the current node.
     *
     * That is, it returns the depth of the node returned in the last
     * call to Next().
     * \return The depth of the current node.
     */
    virtual sparksee::gdb::int32_t GetCurrentDepth() const = 0;

    /**
     * \brief Sets the maximum hops restriction.
     *
     * All paths longer than the maximum hops restriction will be ignored.
     * \param maxhops [in] The maximum  hops restriction.
     * It must be positive or zero. Zero, the default value, means unlimited.
     */
    virtual void SetMaximumHops(sparksee::gdb::int32_t maxhops);

    /**
     * \brief Destructor.
     */
    virtual ~Traversal();
    
protected:
    
    /**
     * \brief Creates a new instance.
     * \param s [in] Session to get the graph from and perform traversal.
     * \param node [in] Node to start traversal from.
     */
    Traversal(sparksee::gdb::Session &s, sparksee::gdb::oid_t node);
    
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
     * \brief The set of excluded nodes.
     */
    sparksee::gdb::Objects *excludedNodes;
    /**
     * \brief The set of excluded edges.
     */
    sparksee::gdb::Objects *excludedEdges;

    
    /**
     * Check that edges had been added.
     */
    void AssertAddedEdges();
    /**
     * Check that nodes had been added.
     */
    void AssertAddedNodes();
    /**
     * Check that the given edge type is valid
     */
    void AssertEdgeType(sparksee::gdb::type_t edgetype);
    /**
     * Check that the given node type is valid
     */
    void AssertNodeType(sparksee::gdb::type_t nodetype);
    /**
     * Check if the given node has an allowed type.
     */
    sparksee::gdb::bool_t IsNodeTypeAllowed(sparksee::gdb::oid_t nodeId);
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
    Traversal(const Traversal & trav);
    /**
     * \brief Non implemented to avoid use of empty constructor.
     */
    Traversal();    
};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_TRAVERSAL_H_
