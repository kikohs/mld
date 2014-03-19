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

#include <locale>
#include <codecvt>
#include <string>

#include <sparksee/gdb/Objects.h>
#include "mld/model/Node.h"

using namespace mld;

Node::Node()
    : GraphObject()
{
    // Invalid node
}

Node::Node( sparksee::gdb::oid_t id, const AttrMap& data )
    : GraphObject(id, data)
{
}

Node::~Node()
{
}

void Node::setWeight( double v )
{
    m_data[Attrs::V[NodeAttr::WEIGHT]].SetDoubleVoid(v);
}

void Node::setLabel( const std::wstring& label )
{
    m_data[Attrs::V[NodeAttr::LABEL]].SetStringVoid(label);
}
