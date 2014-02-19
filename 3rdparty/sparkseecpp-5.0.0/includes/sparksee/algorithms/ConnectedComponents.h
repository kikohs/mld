/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_CONNECTEDCOMPONENTS_H_
#define _SPARKSEE_CONNECTEDCOMPONENTS_H_

/**
 * \addtogroup algorithms
 * @{
 *
 * \file ConnectedComponents.h
 * \brief It contains the definition of ConnectedComponents class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "gdb/common.h"

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
 * \brief ConnectedComponents class.
 * 
 * This class contains the results processed on a Connectivity algorithm.
 *
 * These results contain information related to the connected components found.
 * We must consider that each connected component has a number in order to
 * identify it. These number identifiers are values from 0 to N-1, 
 * where N is the number of different connected components found.
 *
 * When executing any implementation of the Connectivity, it is possible to 
 * indicate whether the results of the execution must be stored
 * persistently using the class Connectivity setMaterializedAttribute
 * method. In case the results are set to be materialized, users can
 * retrieve this data whenever they want, even if the graph has been closed and
 * opened again, just by creating a new instance of this class.
 *
 * Check out the 'Algorithms' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT ConnectedComponents
{
public:

    /**
     * Creates a new instance of {@link ConnectedComponents}.
     *
     * This constructor method can only be called when a previous execution of any
     * implementation of the {@link Connectivity} class has materialized the results
     * in a common attribute type for all the nodes in the graph. For further
     * information about materializing the results processed on any
     * {@link Connectivity} execution see the documentation of the
     * {@link Connectivity#SetMaterializedAttribute} method.
     * @param s [in] Session to get the graph {@link Graph} on which the information
     * will be retrieved just by getting the values contained in the given common
     * attribute type for all the nodes in the graph and processing them.
     * @param materializedattribute [in] The common attribute type for all the nodes
     * in the graph where data will be retrieved in order to process the results
     * related to the connected components found in the graph.
     */
    ConnectedComponents(sparksee::gdb::Session &s, const std::wstring &materializedattribute);

    /**
     * \brief Destructor.
     */
    virtual ~ConnectedComponents();
    

    /**
     * Returns the connected component where the given node belongs to.
     *
     * @param idNode [in] The node identifier for which the connected component
     * identifier where it belongs will be returned.
     * @return The connected component identifier where the given node
     * identifier belongs to.
     */
    sparksee::gdb::int64_t GetConnectedComponent(sparksee::gdb::oid_t idNode);

    /**
     * Returns the number of connected components found in the graph.
     *
     * @return The number of connected components found in the graph.
     */
    sparksee::gdb::int64_t GetCount();


    /**
     * Returns the collection of nodes contained in the given connected
     * component.
     *
     * @param [in] idConnectedComponent The connected component for which
     * the collection of nodes contained in it will be returned.
     * @return The collection of node identifiers contained in the given
     * connected component.
     */
    sparksee::gdb::Objects* GetNodes(sparksee::gdb::int64_t idConnectedComponent);


    /**
     * Returns the number of nodes contained in the given connected component.
     *
     * @param [in] idConnectedComponent The connected component for which
     * the number of nodes contained in it will be returned.
     * @return The number of nodes contained in the given connected component.
     */
    sparksee::gdb::int64_t GetSize(sparksee::gdb::int64_t idConnectedComponent);


private:
    /**
     * \brief Session.
     */
    sparksee::gdb::Session * sess;
    /**
     * \brief Graph.
     */
    sparksee::gdb::Graph * graph;
    /**
     * \brief common attribute where the connected component information is stored.
     */
    sparksee::gdb::attr_t attr_component;
    /**
     * \brief name of the attr_component.
     */
    std::wstring name_attr;

    /**
     * Sets the attribute where the connected component information is stored.
     */
    void SetMaterializedAttribute(const std::wstring &materializedattribute);

    /**
     * Check that the given node identifier is valid.
     */
    void AssertNode(sparksee::gdb::oid_t idNode);

    /**
     * \brief Removes the global attribute where the connectivity information is stored.
     * Should only be used for session attributes.
     */
    void RemoveGlobalAttribute();

    /**
     * Check that the connectec component attribute data is still valid.
     */
    void AssertAttribute();

};


END_SPARKSEE_ALGORITHMS_NAMESPACE

/** @} */
#endif // _SPARKSEE_CONNECTEDCOMPONENTS_H_
