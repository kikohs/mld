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

#include "mld/dao/MLGDao.h"
#include "mld/operator/XCoarsener.h"
#include "mld/operator/XSelector.h"
#include "mld/operator/MultiAdditiveMerger.h"

using namespace mld;
using namespace dex::gdb;

XCoarsener::XCoarsener( Graph* g )
    : AbstractOperator(g)
    , m_sel( new XSelector(g) )
    , m_merger( new MultiAdditiveMerger(g) )
{
}

XCoarsener::~XCoarsener()
{
}

bool XCoarsener::preExec()
{
    // Check current layer node count
    Layer current = m_dao->topLayer();
    if( m_dao->getNodeCount(current) < 2 ) {
        LOG(logERROR) << "XCoarsener::pre_exec: current layer contains less than 2 nodes";
        return false;
    }
    if( !m_sel->rankNodes(current) ) {
        LOG(logERROR) << "XCoarsener::pre_exec: selector rank node failed";
        return false;
    }
    Layer top = m_dao->addLayerOnTop();
    if( top.id() == Objects::InvalidOID ) {
        LOG(logERROR) << "XCoarsener::pre_exec: cannot add layer on top";
        return false;
    }
    return true;
}

bool XCoarsener::exec()
{
    Layer top = m_dao->topLayer();

    while( m_sel->hasNext() ) {
        // Get best node to coarsen
        SuperNode root = m_sel->next();
#ifdef MLD_SAFE
        if( root.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::exec: selectBestNode failed";
            return false;
        }
#endif
        // Select valid neighbors (not flagged)
        ObjectsPtr neighbors = m_sel->neighbors(root);
        // Merge node and edges
        root = m_merger->merge(root, neighbors);
#ifdef MLD_SAFE
        if( root.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::exec: merge failed";
            return false;
        }
#endif
        // Create top node
        SuperNode rootTop = m_dao->addNodeToLayer(top);
#ifdef MLD_SAFE
        if( rootTop.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::exec: addNodeToLayer failed";
            return false;
        }
#endif
        // Link 2 root nodes
        VLink l = m_dao->addVLink(root, rootTop);
#ifdef MLD_SAFE
        if( l.id() == Objects::InvalidOID ) {
            LOG(logERROR) << "XCoarsener::exec: addVLink failed";
            return false;
        }
#endif
        // Flag node as unavailable, update 2-hop neighbors score
        bool ok = m_sel->flagNode(root);
#ifdef MLD_SAFE
        if( !ok ) {
            LOG(logERROR) << "XCoarsener::exec: flagNode failed";
            return false;
        }
#endif
        createHLinksTopLayer(root, rootTop);
    }
    return true;
}

bool XCoarsener::postExec()
{
    return true;
}

void XCoarsener::createHLinksTopLayer( const SuperNode& root, const SuperNode& rootTop )
{
    // TODO
}
