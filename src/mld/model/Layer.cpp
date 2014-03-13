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

#include <sparksee/gdb/Objects.h>

#include "mld/model/Layer.h"

using namespace mld;

Layer::Layer()
    : GraphObject()
{
}

Layer::Layer( sparksee::gdb::oid_t id, const AttrMap& data )
    : GraphObject(id, data)
{
}

Layer::~Layer()
{
}

void Layer::setIsBaseLayer( bool v )
{
    m_data[LayerAttr::IS_BASE].SetBooleanVoid(v);
}

void Layer::setDescription( const std::wstring& des )
{
    m_data[LayerAttr::DESCRIPTION].SetStringVoid(des);
}
