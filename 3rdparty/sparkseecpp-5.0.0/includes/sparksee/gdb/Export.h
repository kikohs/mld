/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_EXPORT_H_
#define _SPARKSEE_EXPORT_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Export.h
 * \brief It contains the declaration of ExportManager interface and GraphExport, 
 * NodeExport, etc classes.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/common.h"
#include "gdb/Exception.h"

#include <map>
#include <iostream>


BEGIN_SPARKSEE_GDB_NAMESPACE


/*
 * Forward declaration of sparksee classes.
 */
class Graph;
class Type;


/**
 * \brief Stores the graph exporting values.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT GraphExport 
{
private:
    
    /**
     * \brief Graph label.
     * 
     * An empty string is the default value.
     */
    std::wstring label;
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    GraphExport(const GraphExport & ge);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    GraphExport & operator =(const GraphExport & ge);

public:
    
    /**
     * \brief Creates a new GraphExport instance.
     */
    GraphExport() 
    {
        SetDefaults();
    }

    /**
     * \brief Destructor.
     */
    virtual ~GraphExport()
    {
    }
    
    /**
     * \brief Sets to default values.
     */
    void SetDefaults();
    
    /**
     * \brief Gets the graph label.
     * \return The graph label.
     */
    const std::wstring & GetLabel() const 
    {
        return label;
    }
    
    /**
     * Sets the graph label.
     * \param l [in] The graph label.
     */
    void SetLabel(const std::wstring & l) 
    {
        label = l;
    }
};

/**
 * \brief Stores the node exporting values.
 *
 * When 'fit' is set to TRUE, then 'height' and 'width' will be ignored.
 *
 * Some properties may be ignored depending on the exportation type.
 *
 * Default values are:
 *
 * Label: "" (empty string).
 *
 * Shape: Box.
 *
 * Color: 10863606 (0xa5c3f6).
 *
 * Label color: 0 (Ox000000, Black).
 *
 * Height: 25px.
 *
 * Width: 25px.
 *
 * Fit: TRUE.
 *
 * Font size: 10.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT NodeExport 
{
private:
    
    /**
     * \brief Node label.
     */
    std::wstring label;
    
    /**
     * \brief Node shape.
     */
    NodeShape shape;
    
    /**
     * \brief Node color.
     */
    ColorRGB color;
    
    /**
     * \brief Node label color.
     */
    ColorRGB colorLabel;
    
    /**
     * \brief Node height.
     */
    int32_t height;
    
    /**
     * \brief Node width.
     */
    int32_t width;
    
    /**
     * \brief Fit the node size to the label or not.
     */
    bool_t fit;
    
    /**
     * \brief Node label font size.
     */
    int32_t fontSize;
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    NodeExport(const NodeExport & ne);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    NodeExport & operator =(const NodeExport & ne);
    
public:
    
    /**
     * \brief Creates a new instance.
     */
    NodeExport() 
    {
        SetDefaults();
    }
    
    /**
     * \brief Destructor.
     */
    virtual ~NodeExport()
    {
    }
    
    /**
     * \brief Sets to default values.
     */
    void SetDefaults();
    
    /**
     * Gets the node label.
     * \return The node label.
     */
    const std::wstring & GetLabel() const 
    {
        return label;
    }
    
    /**
     * Sets the node label.
     * \param l [in] The node label.
     */
    void SetLabel(const std::wstring & l) 
    {
        label = l;
    }
    
    /**
     * \brief Gets the node shape.
     * \return The node shape.
     */
    NodeShape GetShape() const 
    {
        return shape;
    }
    
    /**
     * \brief Sets the node shape.
     * \param s [in] The node shape.
     */
    void SetShape(NodeShape s) 
    {
        shape = s;
    }
    
    /**
     * \brief Gets the node color.
     * \return The node color.
     */
    ColorRGB GetColorRGB() const 
    {
        return color;
    }
    
    /**
     * \brief Sets the node color.
     * \param c The node color.
     */
    void SetColorRGB(ColorRGB c) 
    {
        color = c;
    }
    
    /**
     * \brief Gets the node label color.
     * \return The node label color.
     */
    ColorRGB GetLabelColorRGB() const 
    {
        return colorLabel;
    }
    
    /**
     * \brief Sets the node label color.
     * \param c [in] The node label color.
     */
    void SetLabelColorRGB(ColorRGB c) 
    {
        colorLabel = c;
    }
    
    /**
     * \brief Gets the node height.
     * \return The node height in pixels.
     */
    int32_t GetHeight() const 
    {
        return height;
    }
    
    /**
     * \brief Sets the node height.
     * \param h [in] The node height in pixels.
     */
    void SetHeight(int32_t h) 
    {
        height = h;
    }
    
    /**
     * \brief Gets the node width.
     * \return The node width in pixels.
     */
    int32_t GetWidth() const 
    {
        return width;
    }
    
    /**
     * \brief Gets the node width.
     * \param w The node width in pixels.
     */
    void SetWidth(int32_t w) 
    {
        width = w;
    }
    
    /**
     * \brief Gets whether the node size is fitted to the label
     * or not.
     * \return If TRUE, then the node size
     * is fitted to the label, otherwise the size
     * is fixed with the values of 'height' and 'width'.
     */
    bool_t IsFit() const 
    {
        return fit;
    }
    
    /**
     * \brief Sets the node fit property.
     * \param f [in] If TRUE, then the node size
     * is fitted to the label ('height' and 'width' will be ignored), 
     * otherwise the size is fixed with the values of 'height' and 'width'.
     */
    void SetFit(bool_t f) 
    {
        fit = f;
    }
    
    /**
     * \brief Gets the node label font size.
     * \return The node label font size.
     */
    int32_t GetFontSize() const 
    {
        return fontSize;
    }
    
    /**
     * \brief Sets the node label font size.
     * \param s [in] The node label font size.
     */
    void SetFontSize(int32_t s) 
    {
        fontSize = s;
    }
};


/**
 * \brief Stores edge exporting values.
 *
 * Some properties may be ignored depending on the exportation type.
 *
 * Default values are:
 *
 * Label: "" (empty string).
 *
 * As directed: TRUE.
 *
 * Color: 13882323 (OxD3D3D3, Light gray).
 *
 * Label color: 0 (Ox000000, Black).
 *
 * Width: 5px.
 *
 * Font size: 10.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT EdgeExport 
{
private:
    
    /**
     * \brief Edge label.
     */
    std::wstring label;
    
    /**
     * \brief Directed edge or not.
     */
    bool_t asDirected;
    
    /**
     * \brief Edge color.
     */
    ColorRGB color;
    
    /**
     * \brief Edge label color.
     */
    ColorRGB colorLabel;
    
    /**
     * \brief Edge width.
     */
    int32_t width;
    
    /**
     * \brief Edge label font size.
     */
    int32_t fontSize;
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    EdgeExport(const EdgeExport & ee);
    
    /**
     * \brief Non implemented to avoid use of asignment operator.
     */
    EdgeExport & operator =(const EdgeExport & ee);
    
public:
    
    /**
     * \brief Creates a new instance.
     */
    EdgeExport()
    {
        SetDefaults();
    }
    
    /**
     * \brief Destructor.
     */
    virtual ~EdgeExport()
    {
    }
    
    /**
     * \brief Sets to default values.
     */
    void SetDefaults();
    
    /**
     * \brief Gets the edge label.
     * \return The edge label.
     */
    const std::wstring & GetLabel() const 
    {
        return label;
    }
    
    /**
     * \brief Sets the edge label.
     * \param l [in] The edge label.
     */
    void SetLabel(const std::wstring & l) 
    {
        label = l;
    }
    
    /**
     * \brief Gets if the edge should be managed as directed.
     * 
     * TRUE is the default value.
     * If TRUE, use as directed, otherwise use as undirected.
     * \return The edge direction.
     */
    bool_t AsDirected() const 
    {
        return asDirected;
    }
    
    /**
     * \brief Sets if the edge should be managed as directed.
     * \param b [in] If TRUE, use as directed, otherwise use as undirected.
     */
    void SetAsDirected(bool_t b) 
    {
        asDirected = b;
    }
    
    /**
     * \brief Gets the edge color.
     * \return The edge color.
     */
    ColorRGB GetColorRGB() const 
    {
        return color;
    }
    
    /**
     * \brief Sets the edge color.
     * \param c [in] The edge color.
     */
    void SetColorRGB(ColorRGB c) 
    {
        color = c;
    }
    
    /**
     * \brief Gets the edge label color.
     * \return The edge label color.
     */
    ColorRGB GetLabelColorRGB() const 
    {
        return colorLabel;
    }
    
    /**
     * \brief Sets the edge label color.
     * \param c [in] The edge label color.
     */
    void SetLabelColorRGB(ColorRGB c) 
    {
        colorLabel = c;
    }
    
    /**
     * \brief Gets the edge width.
     * \return The edge width.
     */
    int32_t GetWidth() const 
    {
        return width;
    }
    
    /**
     * \brief Sets the edge width.
     * \param w [in] The edge width.
     */
    void SetWidth(int32_t w) 
    {
        width = w;
    }
    
    /**
     * \brief Gets the edge label font size.
     * \return The edge label font size.
     */
    int32_t GetFontSize() const 
    {
        return fontSize;
    }
    
    /**
     * \brief Sets the edge label font size.
     * \param s [in] The edge label font size.
     */
    void SetFontSize(int32_t s) 
    {
        fontSize = s;
    }
};


/**
 * \brief Defines how to export a graph to an external format.
 *
 * This is an interface which must be implemented by the user. While
 * the export proces, a call for each node or edge type and node or edge
 * object is done to get how to export that element.
 * 
 * It is possible to export a Graph to a diferent fortmats.
 * Nowadays, available formats are defined in the ExportType enum.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT ExportManager 
{
public:
    
    /**
     * \brief Destructor.
     */
    virtual ~ExportManager() {
    }
    
    /**
     * \brief Prepares the graph for the export process.
     * 
     * It is called once before the export process.
     * \param graph Graph to be exported.
     */
    virtual void Prepare(Graph * graph) = 0;
    
    /**
     * \brief Ends the export process.
     * 
     * It is called once after the export process.
     */
    virtual void Release() = 0;
    
    /**
     * \brief Gets the graph export definition.
     * \param graphExport [out] The GraphExport which defines
     * how to export the graph.
     * \return TRUE.
     */
    virtual bool_t GetGraph(GraphExport & graphExport) = 0;
    
    /**
     * \brief Gets the default node export definition for the given node type.
     * 
     * GetNode has a higher priority than this function. That is,
     * only if GetNode returns FALSE, the NodeExport of this 
     * function will be used.
     * \param type [in] Node type identifier.
     * \param nodeExport [out] The NodeExport which defines
     * how to export the nodes of the given type.
     * \return TRUE.
     */
    virtual bool_t GetNodeType(type_t type, NodeExport & nodeExport) = 0;
    
    /**
     * \brief Gets the default node export definition for the given edge type.
     * 
     * GetEdge has a higher priority than this function. That is,
     * only if GetEdge returns FALSE, the EdgeExport of this 
     * function will be used.
     * \param type [in] Edge type identifier.
     * \param edgeExport [out] The EdgeExport which defines
     * how to export the edges of the given type.
     * \return TRUE.
     */
    virtual bool_t GetEdgeType(type_t type, EdgeExport & edgeExport) = 0;
    
    /**
     * \brief Gets the node export definition for the given node.
     * \param node Node identifier.
     * \param nodeExport [out] The NodeExport which defines
     * how to export given node.
     * \return TRUE if the given NodeExport has been updated, otherwise
     * FALSE will be returned and the default NodeExport for the type the node
     * belongs to will be used.
     */
    virtual bool_t GetNode(oid_t node, NodeExport & nodeExport) = 0;
    
    /**
     * \brief Gets the edge export definition for the given edge.
     * \param edge Edge identifier.
     * \param edgeExport [out] The EdgeExport which defines
     * how to export given edge.
     * \return TRUE if the given EdgeExport has been updated, otherwise
     * FALSE will be returned and the default EdgeExport for the type the edge
     * belongs to will be used.
     */
    virtual bool_t GetEdge(oid_t edge, EdgeExport & edgeExport) = 0;

    /**
     * \brief Gets whether a node or edge type must be exported
     * or not.
     * \param type Node or edge type identifier.
     * \return If TRUE all objects of the
     * given type will be exported, otherwise they
     * will not be exported.
     */
    virtual bool_t EnableType(type_t type) = 0;
};


/**
 * \brief Default implementation for ExportManager class.
 * 
 * It uses the default values from GraphExport, NodeExport and EdgeExport to 
 * export all node and edge types.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT DefaultExport 
    : public ExportManager
{
private:
    
    /**
     * \brief Graph to be exported.
     */
    Graph * graph;
    
    /**
     * \brief Caches all node and edge types.
     */
    std::map<type_t,Type *> types;
    
public:
    
    /**
     * \brief Creates a new instance.
     */
    DefaultExport();
    
    /**
     * \brief Destructor.
     */
    virtual ~DefaultExport();
    
    /**
     * \brief Default implementation of the ExportManager class method Prepare
     */
    void Prepare(Graph * graph);
    
    /**
     * \brief Default implementation of the ExportManager class method Release
     */
    void Release();
    
    /**
     * \brief Default implementation of the ExportManager class method GetGraph
     * 
     * This sets the default GraphExport values and "Graph" as the label.
     */
    bool_t GetGraph(GraphExport & graphExport);
    
    /**
     * \brief Default implementation of the ExportManager class method GetNodeType
     *
     * This sets de default NodeExport values.
     */
    bool_t GetNodeType(type_t type, NodeExport & nodeExport);
    
    /**
     * \brief Default implementation of the ExportManager class method GetEdgeType
     *
     * This sets de default EdgeExport values.
     */
    bool_t GetEdgeType(type_t type, EdgeExport & edgeExport);
    
    /**
     * \brief Default implementation of the ExportManager class method GetNode
     * 
     * This sets the default NodeExport values and sets the OID as the label.
     * \return TRUE.
     */
    bool_t GetNode(oid_t node, NodeExport & nodeExport);
    
    /**
     * \brief Default implementation of the ExportManager class method GetEdge
     * 
     * This sets the default EdgeExport values and sets the OID as the label.
     * Also, it exports the edge as directed just if the edge is directed.
     * \return TRUE.
     */
    bool_t GetEdge(oid_t edge, EdgeExport & edgeExport);
    
    /**
     * \brief Default implementation of the ExportManager class method EnableType
     *
     * This enables all node and edge types to be exported.
     * \return TRUE.
     */
    bool_t EnableType(type_t type);
};



END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_EXPORT_H_
