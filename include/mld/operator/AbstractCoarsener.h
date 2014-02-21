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

#ifndef MLD_ABSTRACTCOARSENER_H
#define MLD_ABSTRACTCOARSENER_H

#include "mld/common.h"
#include "mld/operator/AbstractOperator.h"

namespace mld {

class AbstractSingleSelector;
class AbstractSingleMerger;

class AbstractMultiSelector;
class AbstractMultiMerger;

class MLD_API AbstractCoarsener : public AbstractOperator
{
public:
    AbstractCoarsener( sparksee::gdb::Graph* g );
    AbstractCoarsener( sparksee::gdb::Graph* g, float fac );
    virtual ~AbstractCoarsener() = 0;

    /**
     * @brief Scale Factor, for coarsener
     * The factor is always calculated from the Base layer
     * E.g 0.1 means that the goal is to keep 90% of base layer nodes
     * @param fac
     */
    void setReductionFactor( float fac );
    float reductionFactor() const { return m_reductionFac; }
    uint64_t computeMergeCount( int64_t numVertices );

    /**
     * @brief Set coarsening to only one pass by mirroring the layer
     * or false by creating a new layer on the fly but requiring
     * that all nodes from current layer be selected and processed (first pass)
     * before coarsening the top layer (second pass)
     * @param v
     */
    void setSinglePass( bool v ) { m_singlePass = v; }
    bool isSinglePass() const { return m_singlePass; }

protected:
    float m_reductionFac;
    bool m_singlePass;
};

class MLD_API AbstractSingleCoarsener : public AbstractCoarsener
{
public:
    AbstractSingleCoarsener( sparksee::gdb::Graph* g );
    virtual ~AbstractSingleCoarsener() = 0;

protected:
    virtual bool preExec() override;
    virtual bool exec() override;
    virtual bool postExec() override;

protected:
    std::unique_ptr<AbstractSingleSelector> m_sel;
    std::unique_ptr<AbstractSingleMerger> m_merger;
};


} // end namespace mld

#endif // MLD_ABSTRACTCOARSENER_H
