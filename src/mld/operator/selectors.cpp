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

#include "mld/operator/selectors.h"

#include "mld/model/Layer.h"
#include "mld/model/Link.h"
#include "mld/dao/MLGDao.h"

using namespace mld;

HeavyEdgeSelector::HeavyEdgeSelector( dex::gdb::Graph* g )
    : AbstractSingleSelector(g)
{
}

HeavyEdgeSelector::~HeavyEdgeSelector()
{
}


HLink HeavyEdgeSelector::selectBestHLink( const Layer& layer )
{
#ifdef MLD_SAFE
    return m_dao->getHeaviestHLink(layer);
#else
    UNUSED(layer);
    return m_dao->getUnsafeHeaviestLink();
#endif
}
