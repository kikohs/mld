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

typedef std::vector<std::wstring> AttrNameVec;

/**
 * @brief Defines node types
 */
struct MLD_API NodeType
{
    static const std::wstring NODE;
    static const std::wstring LAYER;
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
     * @brief CHILD_OF, Layer - Layer relation
     */
    static const std::wstring CHILD_OF;
};

// NODE ATTRIBUTES
/**
 * @brief Node attributes struct
 */
struct MLD_API NodeAttr
{
    static const std::wstring WEIGHT;
    static const std::wstring LABEL;
    static const AttrNameVec ATTRS;
};

struct MLD_API LayerAttr
{
    static const std::wstring IS_BASE;
    static const std::wstring DESCRIPTION;
};

// EDGE ATTRIBUTES
struct MLD_API H_LinkAttr
{
    static const std::wstring WEIGHT;
    static const AttrNameVec ATTRS;
};

struct MLD_API V_LinkAttr
{
    static const std::wstring WEIGHT;
    static const AttrNameVec ATTRS;
};


} // end namespace mld

/** @} */
#endif // MLD_GRAPHTYPES_H
