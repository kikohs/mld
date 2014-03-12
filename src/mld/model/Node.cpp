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

// Delegatin constructor
Node::Node()
    : Node(sparksee::gdb::Objects::InvalidOID, L"", kSUPERNODE_DEF_VALUE)
{
}

Node::Node( sparksee::gdb::oid_t id )
    : Node(id, L"", kSUPERNODE_DEF_VALUE)
{
}

Node::Node( sparksee::gdb::oid_t id, const std::wstring& label, double weight )
    : GraphObject(id)
{
    // Default construct a Value and set content
    m_data[NodeAttr::LABEL].SetStringVoid(label);
    m_data[NodeAttr::WEIGHT].SetDoubleVoid(weight);
}

Node::Node( sparksee::gdb::oid_t id, const AttrMap& data )
    : GraphObject(id, data)
{
    auto it = m_data.find(NodeAttr::LABEL);
    if( it == m_data.end() )
        m_data[NodeAttr::LABEL].SetStringVoid(L"");

    it = m_data.find(NodeAttr::WEIGHT);
    if( it == m_data.end() )
        m_data[NodeAttr::WEIGHT].SetDoubleVoid(kSUPERNODE_DEF_VALUE);
}

Node::~Node()
{
}

void Node::setWeight( double v )
{
    m_data[NodeAttr::WEIGHT].SetDoubleVoid(v);
}

void Node::setLabel( const std::wstring& label )
{
    m_data[NodeAttr::LABEL].SetStringVoid(label);
}
