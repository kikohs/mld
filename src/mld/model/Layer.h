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

#include "mld/model/GraphObject.h"

namespace mld {

class MLD_API Layer : public GraphObject
{
    friend class LayerDao;
public:
    virtual ~Layer() override;

    inline double isBaseLayer() const { return m_data[LayerAttr::IS_BASE].GetBoolean(); }
    void setDescription( const std::wstring& des );
    std::wstring description() const { return m_data[LayerAttr::DESCRIPTION].GetString(); }

protected:
    Layer();
    explicit Layer( sparksee::gdb::oid_t id, const AttrMap& data );

    void setIsBaseLayer( bool v );
    // "Hide" the setId method
    using GraphObject::setId;
};

} // end namespace mld

#endif // MLD_LAYER_H
