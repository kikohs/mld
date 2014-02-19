/*
 * SPARKSEE: a library for out-oDatabasef-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_GRAPH_H_
#define _SPARKSEE_GRAPH_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Graph.h
 * \brief It contains the definition of Graph class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/Handler.h"
#include "gdb/Graph_data.h"
#include <sstream>

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class DbGraph;
    class GraphTextStream;
    class FileWriter;
}

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 * Forward declaration of sparksee classes.
 */
class Database;
class Session;
class Value;
class Values;
class Objects;
class ExportManager;
class NodeExport;
class EdgeExport;
class TextStream;

/**
 * \brief Graph class.
 *
 * Each Database has a Graph associated, which is the persistent graph which 
 * contains all data stored into the graph database and is retrieved from a
 * Session.
 * 
 * Check out the 'API' and the 'SPARKSEE graph database' sections in the SPARKSEE 
 * User Manual for more details on the use of this class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Graph
: private Handler<sparksee_core::DbGraph>
{
    friend class Session; // to access private constructor
    friend class Values; //to access sparksee_core handled reference
    friend class ValuesIterator; // to access sparksee_core handled reference
    
public:
    
    /**
     * \brief Destructor.
     */
    virtual ~Graph();
    
    /**
     * \brief Creates a new node type.
     * \param name [in] Unique name for the new node type.
     * \return Unique Sparksee type identifier.
     */
    type_t NewNodeType(const std::wstring & name);
    
    /**
     * \brief Creates a new node instance.
     * \param type [in] Sparksee type identifier.
     * \return Unique OID of the new node instance.
     */
    oid_t NewNode(type_t type);
    
    /**
     * \brief Creates a new edge type.
     * \param name [in] Unique name for the new edge type.
     * \param directed [in] If TRUE, this creates a directed edge type, 
     * otherwise this creates a undirected edge type.
     * \param neighbors [in] If TRUE, this indexes neighbor nodes,
     * otherwise not.
     * \return Unique Sparksee type identifier.
     */
    type_t NewEdgeType(const std::wstring & name, bool_t directed, 
                       bool_t neighbors);
    
    /**
     * \brief Creates a new restricted edge type.
     * \param name [in] Unique name for the new edge type.
     * \param tail [in] Tail Sparksee node type identifier.
     * \param head [in] Head Sparksee node type identifier.
     * \param neighbors [in] If TRUE, this indexes neighbor nodes,
     * otherwise not.
     * \return Unique Sparksee type identifier.
     */
    type_t NewRestrictedEdgeType(const std::wstring & name, 
                                 type_t tail, type_t head, bool_t neighbors);
    
    /**
     * \brief Creates a new edge instance.
     * \param type [in] Sparksee type identifier.
     * \param tail [in] Source Sparksee OID.
     * \param head [in] Target Sparksee OID.
     * \return Unique OID of the new edge instance.
     */
    oid_t NewEdge(type_t type, oid_t tail, oid_t head);
    
    /**
     * \brief Creates a new edge instance.
     *
     * The tail of the edge will be any node having the given tailV Value
     * for the given tailAttr attribute identifier, and the head of the edge
     * will be any node having the given headV Value for the given 
     * headAttr attribute identifier.
     * \param type [in] Sparksee type identifier.
     * \param tailAttr [in] Sparksee attribute identifier.
     * \param tailV [in] Value.
     * \param headAttr [in] Sparksee attribute identifier.
     * \param headV [in] Value.
     * \return Unique OID of the new edge instance.
     */
    oid_t NewEdge(type_t type, attr_t tailAttr, Value & tailV,
                  attr_t headAttr, Value & headV);
    
    /**
     * \brief Gets the number of nodes.
     * \return The number of nodes.
     */
    int64_t CountNodes();
    
    /**
     * \brief Gets the number of edges.
     * \return The number of edges.
     */
    int64_t CountEdges();
    
    /**
     * \brief Gets information about an edge.
     * \param edge [in] Sparksee edge identifier.
     * \return An EdgeData instance.
     */
    EdgeData * GetEdgeData(oid_t edge);
    
    /**
     * \brief Gets the other end for the given edge.
     * \param edge [in] Sparksee edge identifier.
     * \param node [in] Sparksee node identifier. It must be one of the
     * ends of the edge.
     * \return The other end of the edge.
     */
    oid_t GetEdgePeer(oid_t edge, oid_t node);
    
    /**
     * \brief Drops the given OID.
     *
     * It also removes its egdges as well as its attribute values.
     * \param oid [in] Sparksee OID to be removed.
     */
    void Drop(oid_t oid);
    
    /**
     * \brief Drops all the OIDs from the given collection.
     * 
     * See Drop method with the single OID parameter. This performs that call
     * for all the elements into the collection.
     * \param objs [in] Objects collection with the OIDs to be removed.
     */
    void Drop(Objects * objs);
    
    /**
     * \brief Gets the Sparksee node or edge type identifier for the given OID.
     * \param oid [in] Sparksee OID.
     * \return Sparksee node or edge type identifier.
     */
    type_t GetObjectType(oid_t oid);
    
    /**
     * \brief Creates a new attribute.
     * \param type [in] Sparksee node or edge type identifier.
     * \param name [in] Unique name for the new attribute.
     * \param dt [in] Data type for the new attribute.
     * \param kind [in] Attribute kind.
     * \return Unique Sparksee attribute identifier.
     */
    attr_t NewAttribute(type_t type, const std::wstring & name, DataType dt,
                        AttributeKind kind);
    
    /**
     * \brief Creates a new attribute with a default value.
     * \param type [in] Sparksee node or edge type identifier.
     * \param name [in] Unique name for the new attribute.
     * \param dt [in] Data type for the new attribute.
     * \param kind [in] Attribute kind.
     * \param defaultValue [in] The default value to use in each new node/edge.
     * \return Unique Sparksee attribute identifier.
     */
    attr_t NewAttribute(type_t type, const std::wstring & name, DataType dt,
                        AttributeKind kind, Value &defaultValue);

    /**
     * \brief Creates a new Session attribute.
     *
     * Session attributes are exclusive for the Session (just its Session
     * can use the attribute) and are automatically removed when the
     * Session is closed (thus, attribute data is not persistent into
     * the database).
     *
     * Since they are not persistent, they cannot be retrieved from the
     * database, so they do not have an identifier name.
     *
     * \param type [in] Sparksee node or edge type identifier.
     * \param dt [in] Data type for the new attribute.
     * \param kind [in] Attribute kind.
     * \return Unique Sparksee attribute identifier.
     */
    attr_t NewSessionAttribute(type_t type, DataType dt, AttributeKind kind);
    

    /**
     * \brief Creates a new Session attribute with a default value.
     *
     * Session attributes are exclusive for the Session (just its Session
     * can use the attribute) and are automatically removed when the
     * Session is closed (thus, attribute data is not persistent into
     * the database).
     *
     * Since they are not persistent, they cannot be retrieved from the
     * database, so they do not have an identifier name.
     *
     * \param type [in] Sparksee node or edge type identifier.
     * \param dt [in] Data type for the new attribute.
     * \param kind [in] Attribute kind.
     * \param defaultValue [in] The default value to use in each new node/edge.
     * \return Unique Sparksee attribute identifier.
     */
    attr_t NewSessionAttribute(type_t type, DataType dt, AttributeKind kind, Value &defaultValue);

    /**
     * \brief Sets a default value for an attribute.
     *
     * The default value will be applied to all the new nodes or edges.
     *
     * The given value must have the same DataType as the attribute or
     * be a NULL value to remove the current default value.
     *
     * \param attr [in] The attribute.
     * \param v [in] The default value to use for this attribute.
     */
    void SetAttributeDefaultValue( attr_t attr, Value & v);

    /**
     * \brief Updates the index of the given attribute.
     *
     * This just works if the current index of the attribute
     * corresponds to the AttributeKind Basic and the new one is
     * Indexed or Unique.
     * \param attr [in] Sparksee attribute identifier.
     * \param kind [in] Attribute kind.
     */
    void IndexAttribute(attr_t attr, AttributeKind kind);
    
    /**
     * \brief Gets the Value for the given attribute and OID.
     * \param oid [in] Sparksee OID.
     * \param attr [in] Sparksee attribute identifier.
     * \param v [in|out] Value for the given attribute and for the
     * given OID.
     */
    void GetAttribute(oid_t oid, attr_t attr, Value & v);
    
    /**
     * \brief Gets the Value for the given attribute and OID.
     * 
     * The other version of this call, where the Value is an output parameter
     * instead of the return, is better because it allows the user to reuse
     * an existing Value instance, whereas this call always creates a new
     * Value instance to be returned.
     *
     * It never returns NULL. Thus, in case the OID has a NULL value for the 
     * attribute it returns a NULL Value instance.
     * 
     * \param oid [in] Sparksee OID.
     * \param attr [in] Sparksee attribute identifier.
     * \return A new Value instance having the attribute value for the given 
     * OID. 
     */
    Value * GetAttribute(oid_t oid, attr_t attr);
    
    /**
     * \brief Gets the read-only TextStream for the given text attribute and OID.
     * \param oid [in] Sparksee OID.
     * \param attr [in] Sparksee attribute identifier.
     * \return A TextStream. This returns a TextStream to read.
     */
    TextStream* GetAttributeText(oid_t oid, attr_t attr);
    
    /**
     * \brief Sets the writable TextStream for the given text attribute and OID.
     * \param oid [in] Sparksee OID.
     * \param attr [in] Sparksee attribute identifier.
     * \param tstream [in] New Text value. This corresponds to a TextStream to
     * write.
     */
    void SetAttributeText(oid_t oid, attr_t attr, TextStream* tstream);
    
    /**
     * \brief Sets the Value for the given attribute and OID.
     * \param oid [in] Sparksee OID.
     * \param attr [in] Sparksee attribute identifier.
     * \param v [in] Value for the given attribute and for the
     * given OID.
     */
    void SetAttribute(oid_t oid, attr_t attr, Value & v);
    
    /**
     * \brief Gets statistics from the given attribute.
     * \param attr [in] Sparksee attribute identifier.
     * \param basic [in] If FALSE all statistics are computed, if TRUE just 
     * those statistics marked as basic will be computed (see description
     * of the AttributeStatistics class). Of course, computing just basic statistics
     * will be faster than computing all of them.
     * \return An AttributeStatistics instace.
     */
    AttributeStatistics * GetAttributeStatistics(attr_t attr, bool_t basic);
    
    /**
     * \brief Gets how many objects have a value into the given 
     * range for the given attribute.
     * 
     * This only works for the attributes with the AttributeKind
     * Indexed or Unique.
     *
     * Given values must belong to the same DataType than the attribute.
     * \param attr [in] Sparksee attribute identifier.
     * \param lower [in] Lower bound Value of the range.
     * \param includeLower [in] If TRUE, include lower bound Value of the range.
     * \param higher [in] Higher bound Value of the range.
     * \param includeHigher [in] If TRUE, include higher bound Value of the range.
     * \return Number of objects having a value into the given range.
     */
    int64_t GetAttributeIntervalCount(attr_t attr,
                                      Value & lower, bool_t includeLower,
                                      Value & higher, bool_t includeHigher);
    
    /**
     * \brief Gets the Sparksee type identifier for the given type name.
     * \param name [in] Unique type name.
     * \return The Sparksee type identifier for the given type name or
     * the Type InvalidType if there is no type with the given name.
     */
    type_t FindType(const std::wstring & name);
    
    /**
     * \brief Gets information about the given type.
     * \param type [in] Sparksee type identifier.
     * \return The Type for the given Sparksee type identifier.
     */
    Type * GetType(type_t type);
    
    /**
     * \brief Removes the given type.
     *
     * This fails if there exist attributes defined for the type
     * or if there exist restricted edges referencing this type.
     * \param type [in] Sparksee type identifier.
     */
    void RemoveType(type_t type);
    

    /**
     * Renames a type.
     *
     * The new name must be available.
     *
     * @param oldName [in] The current name of the type to be renamed.
     * @param newName [in] The new name for the type.
     */
    void RenameType(const std::wstring & oldName, const std::wstring & newName);

    /**
     * Renames a type.
     *
     * The new name must be available.
     *
     * @param type [in] The type to be renamed.
     * @param newName [in] The new name for the type.
     */
    void RenameType(type_t type, const std::wstring & newName);

    /**
     * \brief Gets the Sparksee attribute identifier for the given
     * type identifier and attribute name.
     * \param type [in] Sparksee type identifier.
     * \param name [in] Unique attribute name.
     * \return The Sparksee attribute identifier for the given type and
     * attribute name or InvalidAttribute if there is no attribute
     * with the given name for the given type.
     */
    attr_t FindAttribute(type_t type, const std::wstring & name);
    
    /**
     * \brief Gets information about the given attribute.
     * \param attr [in] Sparksee attribute identifier.
     * \return The Attribute for the given Sparksee attribute identifier.
     */
    Attribute * GetAttribute(attr_t attr);
    
    /**
     * \brief Removes the given attribute.
     * \param attr [in] Sparksee attribute identifier.
     */
    void RemoveAttribute(attr_t attr);

    /**
     * Renames an attribute.
     *
     * The new name must be available.
     *
     * \param attr [in] Sparksee attribute identifier.
     * \param newName [in] The new name for the attribute.
     */
    void RenameAttribute(attr_t attr, const std::wstring & newName);

    /**
     * \brief Finds one object having the given Value for the given
     * attribute.
     *
     * If there are more than one, then any of them will be returned. 
     * And in case there are no object having this Value, the Objects InvalidOID
     * will be returned.
     * \param attr [in] Sparksee attribute identifier.
     * \param v [in] Value.
     * \return Sparksee OID or the Objects InvalidOID.
     */
    oid_t FindObject(attr_t attr, Value & v);
    
    /**
     * \brief Finds one object having the given Value for the
     * attribute or it creates one does not exist any.
     *
     * If the attribute is a node or edge attribute and at least one node/edge with
     * that value is found, it returns one of them. But if it does not exist, then:
     * If it's a node attribute it will create it and set the attribute.
     * If it's an edge attribute it will return the InvalidOID.
     *
     * Using this method with a global attribute will return the InvalidOID.
     *
     * \param attr [in] Sparksee attribute identifier.
     * \param v [in] Value.
     * \return Sparksee OID or the Objects InvalidOID.
     */
    oid_t FindOrCreateObject(attr_t attr, Value & v);

    /**
     * \brief Selects all OIDs belonging to the given type.
     * \param type [in] Sparksee type identifier.
     * \return Objects instance.
     */
    Objects * Select(type_t type);
    
    /**
     * \brief Selects all OIDs satisfying the given condition for the given
     * attribute.
     * \param attr [in] Sparksee attribute identifier.
     * \param cond [in] Condition to be satisfied.
     * \param v [in] Value to be satisfied.
     * \return Objects instance.
     */
    Objects * Select(attr_t attr, Condition cond, const Value & v);
    
    /**
     * \brief Selects all OIDs satisfying the given condition for the given
     * attribute.
     *
     * This allows to perform the Between operation, thus it has
     * two Value arguments.
     * \param attr [in] Sparksee attribute identifier.
     * \param cond [in] Condition to be satisfied. 
     * It must be the Between Condition.
     * \param lower [in] Lower-bound Value to be satisfied.
     * \param higher [in] Higher-bound Value to be satisfied.
     * \return Objects instance.
     */
    Objects * Select(attr_t attr, Condition cond, const Value & lower, const Value & higher);
    
    /**
     * \brief Selects all OIDs satisfying the given condition for the given
     * attribute.
     * \param attr [in] Sparksee attribute identifier.
     * \param cond [in] Condition to be satisfied.
     * \param v [in] Value to be satisfied.
     * \param restriction [in] Objects to limit the select in this set of objects.
     * \return Objects instance.
     */
    Objects * Select(attr_t attr, Condition cond, const Value & v, const Objects* restriction);

    /**
     * \brief Selects all OIDs satisfying the given condition for the given
     * attribute.
     *
     * This allows to perform the Between operation, thus it has
     * two Value arguments.
     * \param attr [in] Sparksee attribute identifier.
     * \param cond [in] Condition to be satisfied.
     * It must be the Between Condition.
     * \param lower [in] Lower-bound Value to be satisfied.
     * \param higher [in] Higher-bound Value to be satisfied.
     * \param restriction [in] Objects to limit the select in this set of objects.
     * \return Objects instance.
     */
    Objects * Select(attr_t attr, Condition cond, const Value & lower, const Value & higher, const Objects* restriction);

    /**
     * \brief Selects all edges from or to the given node OID and for the given
     * edge type.
     * \param oid [in] Sparksee node OID. 
     * \param etype [in] Sparksee edge type identifier.
     * \param dir [in] Direction.
     * \return Objects instance.
     */
    Objects * Explode(oid_t oid, type_t etype, EdgesDirection dir);
    
    /**
     * \brief Selects all edges from or to each of the node OID in the given
     * collection and for the given edge type.
     * \param objs [in] Sparksee node OID collection. 
     * \param etype [in] Sparksee edge type identifier.
     * \param dir [in] Direction.
     * \return Objects instance.
     */
    Objects * Explode(Objects * objs, type_t etype, EdgesDirection dir);
    
    /**
     * \brief Gets the number of edges from or to the given node OID and for 
     * the given edge type.
     * \param oid [in] Sparksee node OID. 
     * \param etype [in] Sparksee edge type identifier.
     * \param dir [in] Direction.
     * \return The number of edges.
     */
    int64_t Degree(oid_t oid, type_t etype, EdgesDirection dir);
    
    /**
     * \brief Selects all neighbor nodes from or to the given node OID 
     * and for the given edge type.
     * \param oid [in] Sparksee node OID. 
     * \param etype [in] Sparksee edge type identifier.
     * \param dir [in] Direction.
     * \return Objects instance.
     */
    Objects * Neighbors(oid_t oid, type_t etype, EdgesDirection dir);
    
    /**
     * \brief Selects all neighbor nodes from or to each of the node OID in
     * the given collection and for the given edge type.
     * \param objs [in] Sparksee node OID collection. 
     * \param etype [in] Sparksee edge type identifier.
     * \param dir [in] Direction.
     * \return Objects instance.
     */
    Objects * Neighbors(Objects * objs, type_t etype, EdgesDirection dir);
    
    /**
     * \brief Gets all the edges of the given type between two given nodes 
     * (tail and head).
     * \param type [in] Sparksee edge type identifier.
     * \param tail [in] Tail node identifier.
     * \param head [in] Head node identifier.
     * \return Objects instance.
     */
    Objects * Edges(type_t etype, oid_t tail, oid_t head);
    
    /**
     * \brief Gets any of the edges of the given type between two given nodes 
     * (tail and head).
     *
     * If there are more than one, then any of them will be returned. 
     * And in case there are no edge between the given tail and head, the Objects InvalidOID
     * will be returned.
     * \param etype [in] Sparksee edge type identifier.
     * \param tail [in] Tail node identifier.
     * \param head [in] Head node identifier.
     * \return Any of the edges or the Objects InvalidOID.
     */
    oid_t FindEdge(type_t etype, oid_t tail, oid_t head);
    
    /**
     * \brief Gets any of the edges of the specified type between two given nodes
     * (tail and head). If it can not find any edge of this type between them
     * it tries to create a new one.
     *
     * \param etype [in] Sparksee edge type identifier.
     * \param tail [in] Tail node identifier.
     * \param head [in] Head node identifier.
     * \return Any of the edges or the Objects InvalidOID.
     */
    oid_t FindOrCreateEdge(type_t etype, oid_t tail, oid_t head);

    /**
     * \brief Gets all the tails from the given edges collection.
     * \param edges [in] Sparksee edge identifier collection.
     * \return The tails collection.
     */
    Objects * Tails(Objects * edges);
    
    /**
     * \brief Gets all the heads from the given edges collection.
     * \param edges [in] Sparksee edge identifier collection.
     * \return The heads collection.
     */
    Objects * Heads(Objects * edges);
    
    /**
     * \brief Gets all the tails and heads from the given edges collection.
     * \param edges [in] Sparksee edge identifier collection.
     * \param tails [in|out] If not NULL, all the tails will be stored here.
     * \param heads [in|out] If not NULL, all the heads will be stored here.
     */
    void TailsAndHeads(Objects * edges, Objects * tails, Objects * heads);
        
    /**
     * \brief Gets all existing Sparksee node type identifiers.
     * \return Sparksee node type identifier list.
     */
    TypeList * FindNodeTypes();
    
    /**
     * \brief Gets all existing Sparksee edge type identifiers.
     * \return Sparksee edge type identifier list.
     */
    TypeList * FindEdgeTypes();
    
    /**
     * \brief Gets all existing Sparksee node and edge type identifiers.
     * \return Sparksee node and edge type identifier list.
     */
    TypeList * FindTypes();
    
    /**
     * \brief Gets all existing Sparksee attribute identifiers for the given
     * type identifier.
     * \param type [in] Sparksee type identifier.
     * \return Sparksee attribute identifier list.
     */
    AttributeList * FindAttributes(type_t type);
    
    /**
     * \brief Gets all Sparksee attribute identifiers with a non-NULL value
     * for the given Sparksee OID.
     * \param oid [in] Sparksee OID.
     * \return Sparksee attribute identifier list.
     */
    AttributeList * GetAttributes(oid_t oid);
    
    /**
     * \brief Gets the Value collection for the given attribute.
     * \param attr [in] Sparksee attribute identifier.
     */
    Values * GetValues(attr_t attr);

    /**
     * \brief Dumps logical data to a file.
     * \param file [in] Output file path.
     * \exception FileNotFoundException If the given file cannot be created.
     */
    void DumpData(const std::wstring & file) 
    throw(sparksee::gdb::FileNotFoundException, sparksee::gdb::Error);
    
    /**
     * \brief Dumps internal storage data to a file.
     * \param file [in] Output file path.
     * \exception FileNotFoundException If the given file cannot be created.
     */
    void DumpStorage(const std::wstring & file) 
    throw(sparksee::gdb::FileNotFoundException, sparksee::gdb::Error);

    /**
     * \brief Exports the {@link Graph}.
     * \param file [in] Output file.
     * \param type [in] Export type.
     * \param e [in] Defines how to do the export for each graph object.
     */
    void Export(const std::wstring & file, ExportType type, ExportManager * e)
    throw(sparksee::gdb::IOException);
    
    /**
     * \brief Dumps all the data to a backup file. See the Sparksee class Restore method.
     * \param file [in] Output backup file path.
     * \exception FileNotFoundException If the given file cannot be created.
     */
    void Backup(const std::wstring & file)
    throw(sparksee::gdb::FileNotFoundException, sparksee::gdb::Error);

private:
    
    /**
     * \brief Creates a new instance.
     * \param Databasedb [in] Parent Database reference.
     * \param s [in] Parent Session reference.
     * \param core_dbg [in] Handled sparksee_core DbGraph reference.
     */
    Graph(sparksee_core::DbGraph * core_dbg, Database * db, Session * s);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Graph(const Graph & g);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    Graph & operator =(const Graph & g);
    
    /**
     * \brief Parent Database reference.
     */
    Database * db;
    /**
     * \brief Parent Session reference.
     */
    Session * sess;
    
    /**
     *
     */
    void exportNode(sparksee_core::FileWriter *fw, ExportType exptype, type_t ntype, const NodeExport * nExp, oid_t node);
    
    /**
     *
     */
    void exportEdge(sparksee_core::FileWriter *fw, ExportType exptype, type_t etype, const EdgeExport * eExp, oid_t edge, oid_t tail, oid_t head);
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_GRAPH_H_
