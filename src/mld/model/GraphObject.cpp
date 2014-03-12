/****************************************************************************
**
** Copyright (C) 2014 EPFL-LTS2
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

#include "mld/model/GraphObject.h"

using namespace mld;

GraphObject::GraphObject()
    : m_id(sparksee::gdb::Objects::InvalidOID)
{
}

GraphObject::GraphObject( sparksee::gdb::oid_t id )
    : m_id(id)
{
}

GraphObject::GraphObject( sparksee::gdb::oid_t id, const AttrMap& data )
    : m_id(id)
    , m_data(data)
{
}

GraphObject::~GraphObject()
{
}

void GraphObject::print( std::ostream& out ) const
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    out << "id:" << m_id << " ";
    for( auto& v: m_data ) {
        std::wstring s;
        out << converter.to_bytes(v.first) << ":"
            << converter.to_bytes(v.second.ToString(s)) << " "
            ;
    }
}

std::ostream& operator<<( std::ostream& out, const GraphObject& obj )
{
    obj.print(out);
    return out;
}
