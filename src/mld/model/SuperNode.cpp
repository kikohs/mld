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

#include <dex/gdb/Objects.h>
#include "mld/model/SuperNode.h"

using namespace mld;

// ****** NODE ****** //

Node::Node()
    : m_id(dex::gdb::Objects::InvalidOID)
{
}

Node::Node( dex::gdb::oid_t id )
    : m_id(id)
{
}

Node::~Node()
{
}


// ****** SUPER NODE ****** //

SuperNode::SuperNode()
    : Node()
    , m_weight(0.0)
    , m_label(L"")
{
}

SuperNode::SuperNode( dex::gdb::oid_t id )
    : Node(id)
    , m_weight(kSUPERNODE_DEF_VALUE)
    , m_label(L"")
{
}

SuperNode::SuperNode( dex::gdb::oid_t id, const std::wstring& label, double weight )
    : Node(id)
    , m_weight(weight)
    , m_label(label)
{
}

SuperNode::~SuperNode()
{
}

std::ostream& operator<<( std::ostream& out, const SuperNode& sn )
{
    out << "id: " << sn.id() << " "
        << "label: " << sn.label().c_str() << " "
        << "weight: " << sn.weight()
        ;
    return out;
}
