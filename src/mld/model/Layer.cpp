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

#include "mld/model/Layer.h"

using namespace mld;

Layer::Layer()
    : m_id(dex::gdb::Objects::InvalidOID)
    , m_parent(dex::gdb::Objects::InvalidOID)
    , m_child(dex::gdb::Objects::InvalidOID)
    , m_isBase(false)
{
}

Layer::Layer( dex::gdb::oid_t id )
    : m_id(id)
    , m_parent(dex::gdb::Objects::InvalidOID)
    , m_child(dex::gdb::Objects::InvalidOID)
    , m_isBase(false)
{
}

Layer::~Layer()
{
}
