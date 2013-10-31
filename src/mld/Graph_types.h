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

#ifndef MLD_GRAPH_TYPES_H
#define MLD_GRAPH_TYPES_H

#include <string>
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

/**
 * @brief Defines node types
 */
struct MLD_API NodeType
{
    // User oriented nodes
    static const std::wstring SUPERNODE;
    static const std::wstring LAYER;
//    static const std::wstring IDGENERATOR;
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
    static const std::wstring H_LINK;
    /**
     * @brief V_LINK
     *  Vertical link, SuperNode - SuperNode in 2 consecutive layers.
     */
    static const std::wstring V_LINK;
    /**
     * @brief OWNS, a Layer OWNS some SuperNodes
     */
    static const std::wstring OWNS;
    /**
     * @brief PARENT_OF, Layer - Layer relation
     */
    static const std::wstring PARENT_OF;
    /**
     * @brief CHILD_OF, Layer - Layer relation
     */
    static const std::wstring CHILD_OF;
};

// NODE ATTRIBUTES
/**
 * @brief SuperNode attributes struct
 */
struct MLD_API SNAttr
{
    static const std::wstring WEIGHT;
};

struct MLD_API LayerAttr {
    static const std::wstring IS_BASE;
    static const std::wstring DESCRIPTION;
};

// EDGE ATTRIBUTES
struct MLD_API H_LinkAttr
{
    static const std::wstring WEIGHT;
};

struct MLD_API V_LinkAttr
{
    static const std::wstring WEIGHT;
};


} // end namespace mld

/** @} */
#endif // MLD_GRAPH_TYPES_H
