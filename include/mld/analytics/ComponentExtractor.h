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

#include "mld/common.h"

namespace sparksee {
namespace gdb {
    class Graph;
    class Value;
    class Objects;
}}

namespace mld {

class MLGDao;

class MLD_API ComponentExtractor
{
public:
    explicit ComponentExtractor( sparksee::gdb::Graph* g );
    ComponentExtractor( const ComponentExtractor& ) = delete;
    ComponentExtractor& operator=( ComponentExtractor ) = delete;
    ~ComponentExtractor();

    inline void setOverrideThreshold( bool override, double value ) { m_override = override; m_alpha = value; }
    bool run();

private:
    std::unique_ptr<MLGDao> m_dao;
    bool m_override;
    double m_alpha;
    ObjectsPtr m_olinkSet;
};

} // end namespace mld

#endif // MLD_COMPONENTEXTRACTOR_H
