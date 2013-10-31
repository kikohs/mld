/****************************************************************************
**
** Copyright (C) 2013 EPFL-LTS2
** Contact: Kirell Benzi (first.last@epfl.ch)
**
** This file is part of MLD.
**
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.md included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements
** will be met: http://www.gnu.org/licenses/
**
****************************************************************************/

#ifndef MLD_GRAPHSCHEME_H
#define MLD_GRAPHSCHEME_H

#include <map>
#include <boost/any.hpp>

#include <dex/gdb/Graph_data.h>

namespace mld {

/**
 * @brief Enum which stores each type of object in the graph
 */
enum ObjectType {
    NODE = 0,
    EDGE,
    RESTRICTED_EDGE
};

typedef std::map<std::wstring, AttributeProperty> AttributeMap;

/**
 * @brief AttributeProperty class defines the type,
 * the kind and the default value (if needed) for a particular attribute
 */
class AttributeProperty
{
public:
    /**
     * @brief Properties ctors
     */
    AttributeProperty( dex::gdb::DataType type, dex::gdb::AttributeKind kind );
    AttributeProperty( dex::gdb::DataType type, dex::gdb::AttributeKind kind, const boost::any& var );

    /**
     * @brief Gets member type
     */
    dex::gdb::DataType type() const { return m_type; }
    /**
     * @brief Gets member kind
     */
    dex::gdb::AttributeKind kind() const { return m_kind; }
    /**
     * @brief Gets member default value
     */
    boost::any defValue() const { return m_defaultValue; }
    /**
     * @brief Set a dex value with the default value stored
     */
    dex::gdb::Value asDexValue();

private:
    dex::gdb::DataType m_type;
    dex::gdb::AttributeKind m_kind;
    boost::any m_defaultValue;
};

/**
 * @brief ObjectProperties class defines the type of object and its scheme
 */
class ObjectProperty
{
public:

    ObjectProperty();
    ObjectProperty( const AttributeMap& map )
        : m_map(map)
    {}

    virtual ~ObjectProperty() = 0;
    /**
     * @brief Gets member type
     */
    ObjectType type() const { return m_type; }
    /**
     * @brief Gets member mapper
     */
    AttributeMap& attributes() { return m_map; }
    const AttributeMap& attributes() const { return m_map; }
    /**
     * @brief Gets member mapper
     */
    void setAttributes( const AttributeMap& map ) { m_map = map; }

protected:
    ObjectType m_type;
    AttributeMap m_map;
};

/**
 * @brief NodeProperties class defines particular properties to create an node type
 */
class NodeProperty : public ObjectProperty
{
public:
    /**
     * @brief NodeProperty ctor
     */
    NodeProperty( const AttributeMap& map )
        : ObjectProperty(map)
        , m_type(NODE)
    {}

};

/**
 * @brief EdgeProperties class defines particular properties to create an edge type
 */
class EdgeProperty : public ObjectProperty
{
public:
    /**
     * @brief EdgeProperties ctors
     */
    EdgeProperty( bool directed, bool indexedNeighbors, const AttributeMap& map )
        : ObjectProperty(map)
        , m_type(EDGE)
        , m_directed(directed)
        , m_indexedNeighbors(indexedNeighbors)
    {}

    // RESTRICTED_EDGE
    EdgeProperty( const std::wstring& tail, const std::wstring& head,
                  bool indexedNeighbors, const AttributeMap& map )
        : ObjectProperty(map)
        , m_type(RESTRICTED_EDGE)
        , m_tail(tail)
        , m_head(head)
        , m_indexedNeighbors(indexedNeighbors)
    {}

    /**
     * @brief Gets member tail
     */
    std::wstring tail() const { return m_tail; }
    /**
     * @brief Gets member head
     */
    std::wstring head() const { return m_head; }
    /**
     * @brief Retrieves whether the edge is directed or not
     */
    bool isDirected() const { return m_directed; }
    /**
     * @brief Retrieves whether neighbors nodes are indexed or not
     */
    bool isNeighborIndexed() const { return m_indexedNeighbors; }

private:
    std::wstring m_tail;
    std::wstring m_head;
    bool m_directed;
    bool m_indexedNeighbors;
};

} // end namespace mld

#endif // MLD_GRAPHSCHEME_H
