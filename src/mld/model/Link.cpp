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

#include "mld/model/Link.h"

using namespace mld;

// ****** LINK ****** //

Link::Link()
    : m_id(dex::gdb::Objects::InvalidOID)
    , m_src(dex::gdb::Objects::InvalidOID)
    , m_tgt(dex::gdb::Objects::InvalidOID)
{
}

Link::Link( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
    : m_id(dex::gdb::Objects::InvalidOID)
    , m_src(src)
    , m_tgt(tgt)
{
}

Link::~Link()
{
}


// ****** HLINK ****** //

HLink::HLink()
    : Link()
    , m_weight(kHLINK_DEF_VALUE)
{
}

HLink::HLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
    : Link(src, tgt)
    , m_weight(kHLINK_DEF_VALUE)
{
}

HLink::HLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight )
    : Link(src, tgt)
    , m_weight(weight)
{
}

HLink::~HLink()
{
}


// ****** VLINK ****** //

VLink::VLink()
    : Link()
    , m_weight(kVLINK_DEF_VALUE)
{
}

VLink::VLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt )
    : Link(src, tgt)
    , m_weight(kVLINK_DEF_VALUE)
{
}

VLink::VLink( dex::gdb::oid_t src, dex::gdb::oid_t tgt, double weight )
    : Link(src, tgt)
    , m_weight(weight)
{
}

VLink::~VLink()
{
}

std::ostream& operator<<( std::ostream& out, const mld::HLink& hlink )
{
    out << "hlink id: " << hlink.id() << " "
        << "src: " << hlink.source() << " "
        << "tgt: " << hlink.target() << " "
        << "w: " << hlink.weight()
          ;
    return out;
}

std::ostream& operator<<( std::ostream& out, const mld::VLink& vlink )
{
    out << "vlink id: " << vlink.id() << " "
        << "src: " << vlink.source() << " "
        << "tgt: " << vlink.target() << " "
        << "w: " << vlink.weight()
          ;
    return out;
}








