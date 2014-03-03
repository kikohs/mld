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

#ifndef MLD_HEAVYHLINKSELECTOR_H
#define MLD_HEAVYHLINKSELECTOR_H

#include "mld/operator/selector/NeighborSelector.h"

namespace mld {

class MLD_API HeavyHLinkSelector : public NeighborSelector
{
    typedef std::pair<double, sparksee::gdb::oid_t> Endpoint;
public:
    HeavyHLinkSelector( sparksee::gdb::Graph* g );
    virtual ~HeavyHLinkSelector() override;
    /**
     * @brief Return heaviest HLink weight from input node
     * @param snid
     * @return HLink weight
     */
    virtual double calcScore( sparksee::gdb::oid_t snid ) override;

    virtual std::string name() const override { return "HeavyHLinkSelector"; }

    /**
     * @brief Get best endpoint, pair of heaviest edge weight and target
     * SuperNode id
     * @param snid source Node
     * @return endpoint
     */
    Endpoint getBestEnpoint( sparksee::gdb::oid_t snid );
protected:
    /**
     * @brief Set current Neighbor with the heaviest HLink endpoint
     */
    virtual void setCurrentBestNeighbors() override;
};

} // end namespace mld

#endif // MLD_HEAVYHLINKSELECTOR_H
