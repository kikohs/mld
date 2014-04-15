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

#ifndef MLD_GRAPHTYPES_H
#define MLD_GRAPHTYPES_H

#include <string>
#include <map>
#include <vector>

#include "mld/common.h"
/**
 * \addtogroup mld
 * @{
 *
 * \file Graph_types
 * \brief Contains the structure of the Graph
 *
 * \author Kirell
 */

namespace mld {

//enum NodeType {
//    NODE,
//    LAYER,
//    NODETYPE_MAX
//};

//enum EdgeType {
//    HLINK = NODETYPE_MAX,
//    VLINK,
//    OLINK,
//    CHILDOF,
//    EDGETYPE_MAX,
//};

typedef std::map<int, std::wstring> Enum2Wstring;

/**
 * @brief Defines node types
 */
struct MLD_API NodeType
{
    static std::wstring NODE;
    static std::wstring LAYER;
};

/**
 * @brief Defines egde/relationship types
 */
struct MLD_API EdgeType
{
    /**
     * @brief H_LINK
     * Horizontal link, SuperNode - SuperNode in the same layer
     */
    static std::wstring HLINK;
    /**
     * @brief V_LINK
     *  Vertical link, SuperNode - SuperNode in 2 consecutive layers.
     */
    static std::wstring VLINK;
    /**
     * @brief OWNS, a Layer OWNS some SuperNodes
     */
    static std::wstring OLINK;
    /**
     * @brief CHILD_OF, Layer - Layer relation
     */
    static std::wstring CHILD_OF;
};

// ***** ATTRIBUTES ***** //

/**
 * @brief Stores name associated to attributes
 */
struct MLD_API Attrs {
    static Enum2Wstring V;
};


/**
 * @brief Node attributes struct
 */
struct MLD_API NodeAttr
{
    enum Attrs
    {
        WEIGHT,
        LABEL,
        NODEATTR_MAX
    };
};

struct MLD_API LayerAttr
{
    enum Attrs
    {
        IS_BASE = NodeAttr::NODEATTR_MAX,
        DESCRIPTION,
        LAYERATTR_MAX
    };
};

// EDGE ATTRIBUTES
struct MLD_API HLinkAttr
{
    enum Attrs
    {
        WEIGHT = LayerAttr::LAYERATTR_MAX,
        HLINKATTR_MAX
    };

};

struct MLD_API VLinkAttr
{
    enum Attrs
    {
        WEIGHT = HLinkAttr::HLINKATTR_MAX,
        VLINKATTR_MAX
    };

};

struct MLD_API OLinkAttr
{
    enum Attrs
    {
        WEIGHT = VLinkAttr::VLINKATTR_MAX,
        OLINKATTR_MAX
    };
};

struct MLD_API CLinkAttr
{
    enum Attrs
    {
        WEIGHT = OLinkAttr::OLINKATTR_MAX,
        CLINKATTR_MAX
    };
};

struct MLD_API VNodeAttr
{
    enum Attrs
    {
        LAYERID = CLinkAttr::CLINKATTR_MAX,
        LAYERPOS,
        SLICEPOS,
        X,
        Y,
        VNODEATTR_MAX
    };
};


} // end namespace mld

/** @} */
#endif // MLD_GRAPHTYPES_H
