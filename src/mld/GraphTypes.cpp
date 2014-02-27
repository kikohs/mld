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

#include "mld/GraphTypes.h"

namespace mld {

// Node Types
const std::wstring NodeType::SUPERNODE(L"MLD_SUPERNODE");
const std::wstring NodeType::LAYER(L"MLD_LAYER");


// Edge Types
const std::wstring EdgeType::H_LINK(L"MLD_H_LINK");
const std::wstring EdgeType::V_LINK(L"MLD_V_LINK");
const std::wstring EdgeType::OWNS(L"MLD_OWNS");
const std::wstring EdgeType::CHILD_OF(L"MLD_CHILD_OF");

// Node Attributes
// SuperNode
const std::wstring SNAttr::WEIGHT(L"MLD_SN_WEIGHT");
const std::wstring SNAttr::LABEL(L"MLD_SN_LABEL");
const std::wstring SNAttr::IS_ROOT(L"MLD_SN_IS_ROOT");
// Layer
const std::wstring LayerAttr::IS_BASE(L"MLD_LAYER_IS_BASE");
const std::wstring LayerAttr::DESCRIPTION(L"MLD_LAYER_DESCRIPTION");

// Edge Attributes
const std::wstring H_LinkAttr::WEIGHT(L"MLD_H_LINK_WEIGHT");
const std::wstring V_LinkAttr::WEIGHT(L"MLD_V_LINK_WEIGHT");

} // end namespace mld
