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

#ifndef MLD_LAYER_H
#define MLD_LAYER_H

#include <dex/gdb/common.h>
#include "mld/common.h"

namespace mld {

class MLD_API Layer
{
    // Access to private variables parent and child
    friend class LayerDao;
public:
    Layer();
    Layer( dex::gdb::oid_t id );
    virtual ~Layer();

    dex::gdb::oid_t parent() const { return m_parent; }
    dex::gdb::oid_t child() const { return m_child; }
    bool isBaseLayer() { return m_isBase; }
    void setDescription( const std::wstring& des ) { m_desc = des; }
    std::wstring description() const { return m_desc; }

protected:
    dex::gdb::oid_t m_id;
    dex::gdb::oid_t m_parent;
    dex::gdb::oid_t m_child;
    bool m_isBase;
    std::wstring m_desc;
};

} // end namespace mld

#endif // MLD_LAYER_H
