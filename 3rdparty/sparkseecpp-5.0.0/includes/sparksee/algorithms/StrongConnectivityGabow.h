/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_STRONGCONNECTIVITYGABOW_H_
#define _SPARKSEE_STRONGCONNECTIVITYGABOW_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file StrongConnectivityGabow.h
 * \brief It contains the definition of StrongConnectivityGabow class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "algorithms/StrongConnectivity.h"
#include <stack>

BEGIN_SPARKSEE_ALGORITHMS_NAMESPACE


/**
 * This class can be used to solve the problem of finding strongly connected
 * components in a <b> directed </b> graph.
 *
 * It consists in finding components where every pair (u,v) of nodes
 * contained in it has a path from u to v using the specified direction for each
 * edge type.  This implementation is based on the Gabow algorithm.
 *
 * It is possible to set some restrictions after constructing a new instance
 * of this class and before running it in order to limit the results.
 *
 * After the execution, we can retrieve the results stored in an instance of the
 * ConnectedComponents class using the GetConnectedComponents method.
 *
 * Check out the 'Algorithms' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT StrongConnectivityGabow : public StrongConnectivity
{
public:

    /**
     * Creates a new instance of {@link StrongConnectivityGabow}.
     * After creating this instance is required to indicate the set of edge types
     * and the set of node types which will be navigated through while traversing
     * the graph in order to find the strong connected components.
     * \param s [in] Session to get the graph from and calculate the connectivity
     */
    StrongConnectivityGabow(sparksee::gdb::Session &s);

    /**
     * \brief Destructor.
     */
    virtual ~StrongConnectivityGabow();
    
    /**
     * \brief Executes the algorithm.
     */
    void Run();

private:

    class InfoNode
    {
    private:
        /** Node to be exploded in the next iterations. */
        sparksee::gdb::oid_t idNod;
        /** The node before the current one. */
        sparksee::gdb::oid_t idNodPred;
        /** Boolean to know if the node must be poped. */
        sparksee::gdb::bool_t isNeededToBePop;

    public:
        InfoNode(sparksee::gdb::oid_t idNode, sparksee::gdb::oid_t idNodePred, sparksee::gdb::bool_t isNeededToBePoped)
        {
            idNod = idNode;
            idNodPred = idNodePred;
            isNeededToBePop = isNeededToBePoped;
        }

        inline sparksee::gdb::oid_t GetIdNode()
        {
            return idNod;
        }

        inline sparksee::gdb::oid_t GetIdNodPred()
        {
            return idNodPred;
        }

        inline sparksee::gdb::bool_t IsNeededTobePoped()
        {
            return isNeededToBePop;
        }
    };

    sparksee::gdb::Objects* nodesStored;
    sparksee::gdb::Objects* nodesVisited;
    std::stack<sparksee::gdb::oid_t> stack1;
    std::stack<sparksee::gdb::oid_t> stack2;
    std::stack<InfoNode*> infoStack;
    /**
     * \brief An auxiliary transient attribute.
     */
    sparksee::gdb::attr_t indexAttribute;
    sparksee::gdb::int32_t index;


    void Gabow(sparksee::gdb::oid_t idNode);
    void SetInfoToNodeNotVisited(sparksee::gdb::oid_t idNodePoped, sparksee::gdb::oid_t idNodePredPoped);
    void SetIndex(sparksee::gdb::oid_t idNode, sparksee::gdb::int32_t index);
    sparksee::gdb::int32_t GetIndex(sparksee::gdb::oid_t idNode);
    void StoreStronglyConnectedComponent(sparksee::gdb::oid_t idNode);
    void VisitNeighborsOfAType(sparksee::gdb::oid_t idNode, sparksee::gdb::type_t edgetype, sparksee::gdb::EdgesDirection edgeDirection);
};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_STRONGCONNECTIVITYGABOW_H_
