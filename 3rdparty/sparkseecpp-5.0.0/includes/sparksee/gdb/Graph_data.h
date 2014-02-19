/*
 * SPARKSEE: a library for out-oDatabasef-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_GRAPH_DATA_H_
#define _SPARKSEE_GRAPH_DATA_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Graph_data.h
 * \brief It contains the definition of some Graph-related classes.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/common.h"
#include "gdb/Value.h"
#include <vector>

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 * Forward declaration of sparksee classes.
 */
class Graph;
class Value;

class Type;
class TypeList;
class TypeListIterator;
class Attribute;
class AttributeList;
class AttributeListIterator;
class OIDList;
class OIDListIterator;
class StringList;
class StringListIterator;
class BooleanList;
class BooleanListIterator;
class Int32List;
class Int32ListIterator;

/**
 * \brief Type data class.
 * 
 * It contains information about a node or edge type.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Type
{
    friend class Graph; // to set private fields
    
private:
    
    /**
     * \brief Sparksee type identifier.
     */
    type_t ident;
    
    /**
     * \brief Object type.
     * \see ObjectType
     */
    ObjectType objType;
    
    /**
     * \brief Unique type name.
     */
    std::wstring name;
    
    /**
     * \brief Number of objects belonging to the type.
     */
    int64_t nObjects;
    
    /**
     * \brief TRUE for directed edges, FALSE otherwise.
     */
    bool_t isDirected;
    
    /**
     * \brief TRUE for restricted edges, FALSE otherwise.
     */
    bool_t isRestricted;
    
    /**
     * \brief TRUE for edges with neighbors index, FALSE otherwise.
     */
    bool_t neighborsIndex;
    
    /**
     * \brief For restricted edges, the tail or source type identifier,
     * the Type InvalidType otherwise.
     */
    type_t restrictedFrom;
    
    /**
     * \brief For restricted edges, the head or target type identifier,
     * the Type InvalidType otherwise.
     */
    type_t restrictedTo;
    
    /**
     * \brief Creates a new instance.
     */
    Type();
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Type(const Type & t);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    Type & operator =(const Type & t);
    
public:
    
    /**
     * \brief Destructor
     */
    ~Type();

    /**
     * \brief Invalid type identifier constant.
     */
    static const type_t InvalidType;
    
    /**
     * \brief Global type identifier constant.
     */
    static const type_t GlobalType;
    
    /**
     * \brief Gets the Sparksee type identifier.
     * \return The Sparksee type identifier.
     */
    type_t GetId() const
    {
        return ident;
    }
    
    /**
     * \brief Gets the object type.
     * \return The object type.
     */
    ObjectType GetObjectType() const
    {
        return objType;
    }
    
    /**
     * \brief Gets the unique type name.
     * \return The unique type name.
     */
    const std::wstring & GetName() const
    {
        return name;
    }
    
    /**
     * \brief Gets the number of objects belonging to the type.
     * \return The number of objects belonging to the type.
     */
    int64_t GetNumObjects() const
    {
        return nObjects;
    }
    
    /**
     * \brief Gets if this is a directed edge type.
     * \return TRUE for directed edge types, FALSE otherwise.
     */
    bool_t GetIsDirected() const
    {
        return isDirected;
    }
    
    /**
     * \brief Gets if this is a restricted edge type.
     * \return TRUE for restricted edge types, FALSE otherwise.
     */
    bool_t GetIsRestricted() const
    {
        return isRestricted;
    }
    
    /**
     * \brief Gets if this is an edge type with neighbors index.
     * \return TRUE for edges types with neighbors index, FALSE otherwise.
     */
    bool_t GetAreNeighborsIndexed() const
    {
        return neighborsIndex;
    }
    
    /**
     * \brief Gets the tail or source type identifier for restricted edge
     * types.
     * \return For restricted edge types, the tail or source type identifier,
     * the Type InvalidType otherwise.
     */
    type_t GetRestrictedFrom() const
    {
        return restrictedFrom;
    }
    
    /**
     * \brief Gets the head or target type identifier for restricted edge
     * types.
     * \return For restricted edge types, the head or target type identifier,
     * the Type InvalidType otherwise.
     */
    type_t GetRestrictedTo() const
    {
        return restrictedTo;
    }
};


/**
 * \brief Sparksee type identifier list.
 *
 * It stores a Sparksee node or edge type identifier list.
 *
 * Use TypeListIterator to access all elements into this collection.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT TypeList
{
public:
    
    /**
     * \brief Number of elements in the list.
     * \return Number of elements in the list.
     */
    int32_t Count() const
    {
        return (int32_t) list.size();
    }
    
    /**
     * \brief Gets a new TypeListIterator.
     * \return TypeListIterator instance.
     */
    TypeListIterator * Iterator(); 
    
    /**
     * \brief Constructor.
     *
     * This creates an empty list.
     */
    TypeList();
    
    /**
     * \brief Constructor.
     * \param v [in] Vector.
     */
    TypeList(const std::vector<type_t> & v);
    
    /**
     * \brief Adds a Sparksee type identifier at the end of the list.
     * \param type [in] Sparksee type identifier.
     */
    void Add(type_t type) 
    {
        list.push_back(type);
    }
    
    /**
     * \brief Clears the list.
     */
    void Clear();
    
    /**
     * \brief Destructor
     */
    ~TypeList();


private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    TypeList(const TypeList & tl);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    TypeList & operator =(const TypeList & tl);
    
    /**
     * \brief Sparksee type identifier list.
     */
    std::vector<type_t> list;
};


/**
 * \brief TypeList iterator class.
 *
 * Iterator to traverse all the Sparksee node or edge type identifiers into a 
 * TypeList instance.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT TypeListIterator
{
    friend class TypeList; // to access private constructor
    
private:
    
    /**
     * \brief std vector iterator.
     */
    std::vector<type_t>::iterator it;
    
    /**
     * \brief Ending std vector iterator.
     */
    std::vector<type_t>::iterator end;
    
    /**
     * \brief Creates a new instance.
     * \param list List to iterate.
     */
    TypeListIterator(std::vector<type_t> & list);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    TypeListIterator(const TypeListIterator & tli);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    TypeListIterator & operator =(const TypeListIterator & tli);
    
public:
    
    /**
     * \brief Destructor
     */
    ~TypeListIterator();

    /**
     * \brief Moves to the next element.
     * \return The next element.
     */
    type_t Next() 
    {
        type_t ret = *it;
        it++;
        return ret;
    }
    
    /**
     * \brief Gets if there are more elements.
     * \return TRUE if there are more elements, FALSE otherwise.
     */
    bool_t HasNext() 
    {
        return (it < end);
    }
};


/**
 * \brief Attribute data class.
 *
 * It contains information about an attribute.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Attribute
{
    friend class Graph; // to set private fields
    
private:
    
    /**
     * \brief Sparksee attribute identifier.
     */
    attr_t ident;
    
    /**
     * \brief Sparksee type identifier.
     */
    type_t type;
    
    /**
     * \brief Unique attribute name.
     */
    std::wstring name;
    
    /**
     * \brief Data type.
     */
    DataType dataType;
    
    /**
     * \brief Number of different values.
     */
    int64_t size;
    
    /**
     * \brief Number of non-NULL values.
     */
    int64_t count;
    
    /**
     * \brief Gets the attribute kind.
     */
    AttributeKind kind;
    
    /**
     * \brief True if it's a session attribute, or false otherwise.
     */
    bool_t isSession;

    /**
     * \brief Creates a new instance.
     */
    Attribute();
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Attribute(const Attribute & a);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    Attribute & operator =(const Attribute & a);
    
public:
    
    /**
     * \brief Destructor
     */
    ~Attribute();

    /**
     * \brief Invalid attribute identifier constant.
     */
    static const attr_t InvalidAttribute;
    
    /**
     * \brief Gets the Sparksee attribute identifier.
     * \return The Sparksee attribute identifier.
     */
    attr_t GetId() const
    {
        return ident;
    }
    
    /**
     * \brief Gets the Sparksee type identifier.
     * \return The Sparksee type identifier.
     */
    type_t GetTypeId() const
    {
        return type;
    }
    
    /**
     * \brief Gets the unique attribute name.
     * \return The unique attribute name.
     */
    const std::wstring & GetName() const
    {
        return name;
    }
    
    /**
     * \brief Gets the data type.
     * \return The DataType.
     */
    DataType GetDataType() const
    {
        return dataType;
    }
    
    /**
     * \brief Gets the number of different values.
     * \return The number of different values.
     */
    int64_t GetSize() const
    {
        return size;
    }
    
    /**
     * \brief Gets the number of non-NULL values.
     * \return The number of non-NULL values.
     */
    int64_t GetCount() const
    {
        return count;
    }
    
    /**
     * \brief Gets the attribute kind.
     * \return The AttributeKind.
     */
    AttributeKind GetKind() const
    {
        return kind;
    }

    /**
     * \brief Check if it's a session attribute or a persistent one.
     * \return True if it's a session attribute, or false otherwise.
     */
    bool_t IsSessionAttribute() const
    {
       return isSession;
    }
};


/**
 * \brief Sparksee attribute identifier list.
 *
 * It stores a Sparksee attribute identifier list.
 *
 * Use AttributeListIterator to access all elements into this collection.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT AttributeList
{
public:
    
    /**
     * \brief Number of elements in the list.
     * \return Number of elements in the list.
     */
    int32_t Count() const
    {
        return (int32_t) list.size();
    }
    
    /**
     * \brief Gets a new AttributeListIterator.
     * \return AttributeListIterator instance.
     */
    AttributeListIterator * Iterator(); 
    
    /**
     * \brief Constructor.
     *
     * This creates an empty list.
     */
    AttributeList();
    
    /**
     * \brief Constructor.
     * \param v [in] Vector.
     */
    AttributeList(const std::vector<attr_t> & v);
    
    /**
     * \brief Adds a Sparksee attribute identifier at the end of the list.
     * \param attr [in] Sparksee attribute identifier.
     */
    void Add(attr_t attr) 
    {
        list.push_back(attr);
    }
    
    /**
     * \brief Clears the list.
     */
    void Clear();
    

    /**
     * \brief Destructor
     */
    ~AttributeList();

private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    AttributeList(const AttributeList & al);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    AttributeList & operator =(const AttributeList & al);
    
    /**
     * \brief Sparksee attribute identifier list.
     */
    std::vector<attr_t> list;
};


/**
 * \brief AttributeList iterator class.
 *
 * Iterator to traverse all the Sparksee attribute identifier into a AttributeList instance.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT AttributeListIterator
{
    friend class AttributeList; // to access private constructor
    
private:
    
    /**
     * \brief std vector iterator.
     */
    std::vector<attr_t>::iterator it;
    
    /**
     * \brief Ending std vector iterator.
     */
    std::vector<attr_t>::iterator end;
    
    /**
     * \brief Creates a new instance.
     * \param list List to iterate.
     */
    AttributeListIterator(std::vector<attr_t> & list);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    AttributeListIterator(const AttributeListIterator & ali);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    AttributeListIterator & operator =(const AttributeListIterator & ali);
    
public:
    
    /**
     * \brief Destructor
     */
    ~AttributeListIterator();


    /**
     * \brief Moves to the next element.
     * \return The next element.
     */
    attr_t Next() 
    {
        attr_t ret = *it;
        it++;
        return ret;
    }
    
    /**
     * \brief Gets if there are more elements.
     * \return TRUE if there are more elements, FALSE otherwise.
     */
    bool_t HasNext() 
    {
        return (it < end);
    }
};


/**
 * \brief Sparksee object identifier list.
 *
 * It stores a Sparksee object identifier list.
 *
 * Use OIDListIterator to access all elements into this collection.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT OIDList
{
public:
    
    /**
     * \brief Number of elements in the list.
     * \return Number of elements in the list.
     */
    int32_t Count() const
    {
        return (int32_t) list.size();
    }
    
    /**
     * \brief Gets a new OIDListIterator.
     * \return OIDListIterator instance.
     */
    OIDListIterator * Iterator(); 
    
    /**
     * \brief Constructor.
     *
     * This creates an empty list.
     */
    OIDList();
    
    /**
     * \brief Constructor.
     *
     * This creates a list with N invalid oids.
     */
    OIDList(int32_t N);

    /**
     * \brief Constructor.
     * \param v [in] Vector.
     */
    OIDList(const std::vector<oid_t> & v);
    
    /**
     * \brief Destructor
     */
    ~OIDList();


    /**
     * \brief Adds a Sparksee object identifier at the end of the list.
     * \param attr [in] Sparksee object identifier.
     */
    void Add(oid_t attr) 
    {
        list.push_back(attr);
    }
    
    /**
     * \brief Sets a Sparksee object identifier at the specified position of the list.
     * \param pos [in] List position [0..Count()-1].
     * \param oid [in] Sparksee object identifier.
     */
    void Set(int32_t pos, oid_t oid)
    {
        list[pos] = oid;
    }

    /**
     * \brief Clears the list.
     */
    void Clear();
    
private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    OIDList(const OIDList & ol);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    OIDList & operator =(const OIDList & ol);
    
    /**
     * \brief Sparksee object identifier list.
     */
    std::vector<oid_t> list;
};


/**
 * \brief OIDList iterator class.
 *
 * Iterator to traverse all the Sparksee object identifier into a OIDList instance.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT OIDListIterator
{
    friend class OIDList; // to access private constructor
    
private:
    
    /**
     * \brief std vector iterator.
     */
    std::vector<oid_t>::iterator it;
    
    /**
     * \brief Ending std vector iterator.
     */
    std::vector<oid_t>::iterator end;
    
    /**
     * \brief Creates a new instance.
     * \param list List to iterate.
     */
    OIDListIterator(std::vector<oid_t> & list);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    OIDListIterator(const OIDListIterator & oli);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    OIDListIterator & operator =(const OIDListIterator & oli);
    
public:
    
    /**
     * \brief Destructor
     */
    ~OIDListIterator();

    /**
     * \brief Moves to the next element.
     * \return The next element.
     */
    oid_t Next() 
    {
        oid_t ret = *it;
        it++;
        return ret;
    }
    
    /**
     * \brief Gets if there are more elements.
     * \return TRUE if there are more elements, FALSE otherwise.
     */
    bool_t HasNext() 
    {
        return (it < end);
    }
};


/**
 * \brief Attribute statistics class.
 * 
 * It contains statistic data about an attribute.
 *
 * Some fields are valid just for numerical attributes and others just
 * for string attributes. Also, some statistics are considered BASIC because
 * computing them do not require to traverse all the different values of the 
 * attribute. For each getter method the documentation tells if the statistic 
 * is BASIC or not. See the Graph class method getAttributeStatistics or check
 * out the SPARKSEE User Manual for more details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT AttributeStatistics
{
    friend class Graph; // to set private fields
    
private:
    
    /**
     * \brief Number of objects with a non-NULL Value.
     */
    int64_t total;
    
    /**
     * \brief Number of objects NULL a Value.
     */
    int64_t nul;
    
    /**
     * \brief Number of distinct values.
     */
    int64_t distinct;
    
    /**
     * \brief Minimum existing value.
     */
    Value minV;
    
    /**
     * \brief Maximum existing value.
     */
    Value maxV;
    
    /**
     * \brief Mode: Most frequent Value.
     */
    Value modeV;
    
    /**
     * \brief In case of being an string attribute, this is the maximum length.
     */
    int32_t maxLengthStr;
    
    /**
     * \brief In case of being an string attribute, this is the minimum length.
     */
    int32_t minLengthStr;
    
    /**
     * \brief In case of being an string attribute, this is the average length.
     */
    double64_t avgLengthStr;
    
    /**
     * \brief Number of objects with a Value equal to the mode, that is
     * the most frequent Value.
     */
    int64_t modeCount;
    
    /**
     * \brief Mean or average:  Sum of all Values divided by the number of 
     * observations.
     *
     * It is computed just for numerical attributes.
     */
    double64_t mean;
    
    /**
     * \brief Variance.
     *
     * It is computed just for numerical attributes.
     */
    double64_t variance;
    
    /**
     * \brief Median
     * Middle value that separates the higher half from the lower.
     * 
     * If a < b < c, then the median of the list {a, b, c} is b,
     * and if a < b < c < d, then the median of the list {a, b, c, d}
     * is the mean of b and c, i.e. it is (b + c)/2
     *
     * It is computed just for numerical attributes.
     */
    double64_t median;
    
    /**
     * \brief Creates a new instance.
     *
     * Values will be set to NULL by default.
     */
    AttributeStatistics()
    {
    }
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    AttributeStatistics(const AttributeStatistics & attrStats);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    AttributeStatistics & operator =(const AttributeStatistics & attrStats);
    
public:
    
    /**
     * \brief Destructor
     */
    ~AttributeStatistics()
    {
    }

    /**
     * \brief Gets the number of objects with a non-NULL Value
     * (BASIC statistic).
     * \return The number of objects with a non-NULL Value.
     */
    int64_t GetTotal() const
    {
        return total;
    }
    
    /**
     * \brief Gets the number of objects NULL a Value (BASIC statistics).
     * \return The number of objects NULL a Value.
     */
    int64_t GetNull() const
    {
        return nul;
    }
    
    /**
     * \brief Gets the number of distinct values (BASIC statistics).
     * \return The number of distinct values.
     */
    int64_t GetDistinct() const
    {
        return distinct;
    }
    
    /**
     * \brief Gets the minimum existing value (BASIC statistics).
     * \return The minimum existing value.
     */
    const Value & GetMin() const
    {
        return minV;
    }
    
    /**
     * \brief Gets the maximum existing value (BASIC statistics).
     * \return The maximum existing value.
     */
    const Value & GetMax() const
    {
        return maxV;
    }
    
    /**
     * \brief Gets the maximum length.
     *
     * If the attribute is not an string attribute, it just returns 0.
     *
     * \return The maximum length.
     */
    int32_t GetMaxLengthString() const 
    {
        return maxLengthStr;
    }
    
    /**
     * \brief Gets the minimum length.
     *
     * If the attribute is not an string attribute, it just returns 0.
     *
     * \return The minimum length.
     */
    int32_t GetMinLengthString() const 
    {
        return minLengthStr;
    }
    
    /**
     * \brief Gets the average length.
     *
     * If the attribute is not an string attribute, it just returns 0.
     *
     * \return The average length.
     */
    double64_t GetAvgLengthString() const 
    {
        return avgLengthStr;
    }
    
    /**
     * \brief Gets the mode
     * 
     * Mode: Most frequent Value.
     * \return The mode.
     */
    const Value & GetMode() const
    {
        return modeV;
    }
    
    /**
     * \brief Gets the number of objects with a Value equal to the mode.
     * \return The number of objects with a Value equal to the mode.
     */
    int64_t GetModeCount() const
    {
        return modeCount;
    }
    
    /**
     * \brief Gets the mean or average.
     *
     * Mean or average:  Sum of all Values divided by the number of 
     * observations.
     *
     * It is computed just for numerical attributes.
     * \return The mean.
     */
    double64_t GetMean() const
    {
        return mean;
    }
    
    /**
     * \brief Gets the variance.
     *
     * It is computed just for numerical attributes.
     * \return The variance.
     */
    double64_t GetVariance() const
    {
        return variance;
    }
    
    /**
     * \brief Gets the median.
     *
     * Median: Middle value that separates the higher half from the lower.
     * 
     * If a < b < c, then the median of the list {a, b, c} is b,
     * and if a < b < c < d, then the median of the list {a, b, c, d}
     * is the mean of b and c, i.e. it is (b + c)/2
     *
     * It is computed just for numerical attributes.
     * \return The median.
     */
    double64_t GetMedian() const
    {
        return median;
    }
};


/**
 * \brief Edge data class.
 *
 * It stores the tail and the head of an edge instance.
 * 
 * In case of undirected eges, the tail and the head are just the 
 * two ends of the edge.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT EdgeData
{
    friend class Graph; // to set private fields
    
private:
    
    /**
     * \brief Sparksee edge identifier.
     */
    oid_t edge;
    
    /**
     * \brief Sparksee edge identifier of the tail of the edge.
     */
    oid_t tail;
    
    /**
     * \brief Sparksee edge identifier of the head of the edge.
     */
    oid_t head;
    
    /**
     * \brief Creates a new instance.
     */
    EdgeData();
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    EdgeData(const EdgeData & ed);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    EdgeData & operator =(const EdgeData & ed);
    
public:
    
    /**
     * \brief Destructor
     */
    ~EdgeData();

    /**
     * \brief Gets the edge identifier.
     * \return The Sparksee edge identifier.
     */
    oid_t GetEdge() const
    {
        return edge;
    }
    
    /**
     * \brief Gets the tail of the edge.
     * \return The Sparksee edge identifier of the tail of the edge.
     */
    oid_t GetTail() const
    {
        return tail;
    }
    
    /**
     * \brief Gets the head of the edge.
     * \return The Sparksee edge identifier of the head of the edge.
     */
    oid_t GetHead() const
    {
        return head;
    }
};


/**
 * \brief String list.
 *
 * It stores a String (unicode) list.
 *
 * Use StringListIterator to access all elements into this collection.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT StringList
{
public:
    
    /**
     * \brief Number of elements in the list.
     * \return Number of elements in the list.
     */
    int32_t Count() const
    {
        return (int32_t) list.size();
    }
    
    /**
     * \brief Gets a new StringListIterator.
     * \return StringListIterator instance.
     */
    StringListIterator * Iterator(); 
    
    /**
     * \brief Constructor.
     *
     * This creates an empty list.
     */
    StringList();
    
    /**
     * \brief Constructor.
     * \param v [in] Vector.
     */
    StringList(const std::vector<std::wstring> & v);
    
    /**
     * \brief Destructor
     */
    ~StringList();

    /**
     * \brief Adds a String at the end of the list.
     * \param str [in] String.
     */
    void Add(const std::wstring & str) 
    {
        list.push_back(str);
    }
    
    /**
     * \brief Clears the list.
     */
    void Clear();
    
private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    StringList(const StringList & sl);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    StringList & operator =(const StringList & sl);
    
    /**
     * \brief String list.
     */
    std::vector<std::wstring> list;
};


/**
 * \brief StringList iterator class.
 *
 * Iterator to traverse all the strings into a StringList instance.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT StringListIterator
{
    friend class StringList; // to access private constructor
    
private:
    
    /**
     * \brief std vector iterator.
     */
    std::vector<std::wstring>::iterator it;
    
    /**
     * \brief Ending std vector iterator.
     */
    std::vector<std::wstring>::iterator end;
    
    /**
     * \brief Creates a new instance.
     * \param list List to iterate.
     */
    StringListIterator(std::vector<std::wstring> & list);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    StringListIterator(const StringListIterator & sli);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    StringListIterator & operator =(const StringListIterator & sli);
    
public:
    
    /**
     * \brief Destructor
     */
    ~StringListIterator();

    /**
     * \brief Moves to the next element.
     * \return The next element.
     */
    const std::wstring & Next() 
    {
        const std::wstring & ret = *it;
        it++;
        return ret;
    }
    
    /**
     * \brief Gets if there are more elements.
     * \return TRUE if there are more elements, FALSE otherwise.
     */
    bool_t HasNext() 
    {
        return (it < end);
    }
};


/**
 * \brief Boolean list.
 *
 * It stores a Boolean list.
 *
 * Use BooleanListIterator to access all elements into this collection.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT BooleanList
{
public:
    
    /**
     * \brief Number of elements in the list.
     * \return Number of elements in the list.
     */
    int32_t Count() const
    {
        return (int32_t) list.size();
    }
    
    /**
     * \brief Gets a new BooleanListIterator.
     * \return BooleanListIterator instance.
     */
    BooleanListIterator * Iterator(); 
    
    /**
     * \brief Constructor.
     *
     * This creates an empty list.
     */
    BooleanList();
    
    /**
     * \brief Constructor.
     * \param v [in] Vector.
     */
    BooleanList(const std::vector<bool_t> & v);
    
    /**
     * \brief Destructor
     */
    ~BooleanList();

    /**
     * \brief Adds a Boolean at the end of the list.
     * \param b [in] Boolean.
     */
    void Add(sparksee::gdb::bool_t b) 
    {
        list.push_back(b);
    }
    
    /**
     * \brief Clears the list.
     */
    void Clear();
    
private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    BooleanList(const BooleanList & bl);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    BooleanList & operator =(const BooleanList & bl);
    
    /**
     * \brief Boolean list.
     */
    std::vector<sparksee::gdb::bool_t> list;
};


/**
 * \brief BooleanList iterator class.
 *
 * Iterator to traverse all the strings into a BooleanList instance.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT BooleanListIterator
{
    friend class BooleanList; // to access private constructor
    
private:
    
    /**
     * \brief std vector iterator.
     */
    std::vector<sparksee::gdb::bool_t>::iterator it;
    
    /**
     * \brief Ending std vector iterator.
     */
    std::vector<sparksee::gdb::bool_t>::iterator end;
    
    /**
     * \brief Creates a new instance.
     * \param list List to iterate.
     */
    BooleanListIterator(std::vector<sparksee::gdb::bool_t> & list);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    BooleanListIterator(const BooleanListIterator & bli);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    BooleanListIterator & operator =(const BooleanListIterator & bli);
    
public:
    
    /**
     * \brief Destructor
     */
    ~BooleanListIterator();

    /**
     * \brief Moves to the next element.
     * \return The next element.
     */
    sparksee::gdb::bool_t Next() 
    {
        sparksee::gdb::bool_t ret = *it;
        it++;
        return ret;
    }
    
    /**
     * \brief Gets if there are more elements.
     * \return TRUE if there are more elements, FALSE otherwise.
     */
    bool_t HasNext() 
    {
        return (it < end);
    }
};





/**
 * \brief Sparksee 32-bit signed integer list.
 *
 * It stores a 32-bit signed integer list.
 *
 * Use Int32ListIterator to access all elements into this collection.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Int32List
{
public:

    /**
     * \brief Number of elements in the list.
     * \return Number of elements in the list.
     */
    int32_t Count() const
    {
        return (int32_t) list.size();
    }

    /**
     * \brief Gets a new Int32ListIterator.
     * \return Int32ListIterator instance.
     */
    Int32ListIterator * Iterator();

    /**
     * \brief Constructor.
     *
     * This creates an empty list.
     */
    Int32List();

    /**
     * \brief Constructor.
     * \param v [in] Vector.
     */
    Int32List(const std::vector<int32_t> & v);
    
    /**
     * \brief Destructor
     */
    ~Int32List();

    /**
     * \brief Adds an 32-bit signed integer at the end of the list.
     * \param type [in] The integer.
     */
    void Add(int32_t value)
    {
        list.push_back(value);
    }

    /**
     * \brief Clears the list.
     */
    void Clear();

private:

    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Int32List(const Int32List & il);

    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    Int32List & operator =(const Int32List & il);

    /**
     * \brief Sparksee 32-bit signed integer list.
     */
    std::vector<int32_t> list;
};


/**
 * \brief Int32List iterator class.
 *
 * Iterator to traverse all the integer into a Int32List instance.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Int32ListIterator
{
    friend class Int32List; // to access private constructor

private:

    /**
     * \brief std vector iterator.
     */
    std::vector<int32_t>::iterator it;

    /**
     * \brief Ending std vector iterator.
     */
    std::vector<int32_t>::iterator end;

    /**
     * \brief Creates a new instance.
     * \param list List to iterate.
     */
    Int32ListIterator(std::vector<int32_t> & list);

    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Int32ListIterator(const Int32ListIterator & ili);

    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    Int32ListIterator & operator =(const Int32ListIterator & ili);

public:

    /**
     * \brief Destructor
     */
    ~Int32ListIterator();

    /**
     * \brief Moves to the next element.
     * \return The next element.
     */
    int32_t Next()
    {
        int32_t ret = *it;
        it++;
        return ret;
    }

    /**
     * \brief Gets if there are more elements.
     * \return TRUE if there are more elements, FALSE otherwise.
     */
    bool_t HasNext()
    {
        return (it < end);
    }
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_GRAPH_DATA_H_
