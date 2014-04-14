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

#ifndef MLD_COMPONENTEXTRACTOR_H
#define MLD_COMPONENTEXTRACTOR_H

#include <sparksee/gdb/common.h>
#include "mld/common.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
    class OIDList;
}}

namespace mld {

class MLGDao;
class Layer;

class MLD_API ComponentExtractor
{
public:
    explicit ComponentExtractor( sparksee::gdb::Graph* g );
    ComponentExtractor( const ComponentExtractor& ) = delete;
    ComponentExtractor& operator=( ComponentExtractor ) = delete;
    ~ComponentExtractor();

    inline void setOverrideThreshold( bool override, double value ) { m_override = override; m_alpha = value; }
    bool run();

    /**
     * @brief Compute threashold if not overrided by user
     * @return
     */
    double computeThreshold();

    /**
     * @brief Filter nodes of a layer return only those whose TS value for the input layer
     * is above threshold or inferior to - threshold
     * @param layer Current layer
     * @param threshold
     * @return nodeset
     */
    ObjectsPtr filterNodes( const Layer& layer, double threshold );

    void addVirtualNodes( const ObjectsPtr& nodes );
    void addVirtualVLinks( const sparksee::gdb::OIDList& srcs, const sparksee::gdb::OIDList& tgts );

private:
    std::unique_ptr<MLGDao> m_dao;
    bool m_override;
    double m_alpha;

};

} // end namespace mld

#endif // MLD_COMPONENTEXTRACTOR_H