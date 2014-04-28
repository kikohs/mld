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
std::wstring NodeType::NODE(L"MLD_NODE");
std::wstring NodeType::LAYER(L"MLD_LAYER");

// Edge Types
std::wstring EdgeType::HLINK(L"MLD_HLINK");
std::wstring EdgeType::VLINK(L"MLD_VLINK");
std::wstring EdgeType::OLINK(L"MLD_OLINK");
std::wstring EdgeType::CHILD_OF(L"MLD_CHILD_OF");

// Attributes
Enum2Wstring Attrs::V {
    {NodeAttr::WEIGHT, L"MLD_N_WEIGHT"},
    {NodeAttr::LABEL, L"MLD_N_LABEL"},
    {LayerAttr::IS_BASE, L"MLD_LAYER_IS_BASE"},
    {LayerAttr::DESCRIPTION, L"MLD_LAYER_DESCRIPTION"},
    {HLinkAttr::WEIGHT, L"MLD_HLINK_WEIGHT"},
    {VLinkAttr::WEIGHT, L"MLD_VLINK_WEIGHT"},
    {OLinkAttr::WEIGHT, L"MLD_OLINK_WEIGHT"},
    {CLinkAttr::WEIGHT, L"MLD_CLINK_WEIGHT"},
    {DyNodeAttr::LAYERID, L"MLD_DYNODE_LAYERID"},
    {DyNodeAttr::BASEID, L"MLD_DYNODE_BASEID"},
    {DyNodeAttr::LAYERPOS, L"MLD_DYNODE_LAYERPOS"},
    {DyNodeAttr::SLICEPOS, L"MLD_DYNODE_SLICEPOS"},
    {DyNodeAttr::X, L"MLD_DYNODE_X"},
    {DyNodeAttr::Y, L"MLD_DYNODE_Y"},
    {DyNodeAttr::SIZE, L"MLD_DYNODE_SIZE"},
    {DyNodeAttr::COLOR, L"MLD_VNODE_COLOR"},
    {DyNodeAttr::INPUTID, L"MLD_DYNODE_INPUTID"},
    {DyNodeAttr::COMPONENTID, L"MLD_DYNODE_COMPONENTID"},
    {DyNodeAttr::COMPONENTID, L"MLD_DYNODE_COMPONENTNUM"},
};

} // end namespace mld
