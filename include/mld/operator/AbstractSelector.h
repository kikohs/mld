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

#ifndef MLD_ABSTRACTSELECTOR_H
#define MLD_ABSTRACTSELECTOR_H

#include "mld/common.h"
#include "mld/model/Link.h"
#include "mld/model/Layer.h"
#include "mld/model/SuperNode.h"

namespace dex {
namespace gdb {
    class Graph;
}}
namespace mld {
// Forward declaration
class MLGDao;

/**
 * @brief Base class inherited by all edge selectors
 * Provides a common interface for all mergers.
 * Used by all coarseners
 */
class MLD_API AbstractSelector
{
public:
    AbstractSelector( dex::gdb::Graph* g );
    virtual ~AbstractSelector() = 0;

    // Disable copy and assignement ctor
    AbstractSelector( const AbstractSelector& ) = delete;
    AbstractSelector& operator=( const AbstractSelector& ) = delete;

protected:
    std::unique_ptr<MLGDao> m_dao;
};

// Single edge selector
class MLD_API AbstractSingleSelector: public AbstractSelector
{
public:
    AbstractSingleSelector( dex::gdb::Graph* g );
    virtual ~AbstractSingleSelector() = 0;
    virtual HLink selectBestHLink( const Layer& layer ) = 0;
};

// Multi edge selector
class MLD_API AbstractMultiSelector: public AbstractSelector
{
public:
    AbstractMultiSelector( dex::gdb::Graph* g );
    virtual ~AbstractMultiSelector() = 0;

    virtual SuperNode selectBestNode( const Layer& layer ) = 0;
};

} // end namespace mld

#endif // MLD_ABSTRACTSELECTOR_H
