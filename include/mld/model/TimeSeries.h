
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

#ifndef MLD_TIMESERIES_H
#define MLD_TIMESERIES_H

#include <vector>

#include "mld/common.h"

namespace mld {

class MLD_API TimeSeries
{
public:
    using TSData = std::vector<double>;
    using TSIt = TSData::iterator;

    enum class TSDirection {
        BACKWARD,
        FORWARD,
        BOTH
    };

    TimeSeries();
    virtual ~TimeSeries();

    TSIt begin();
    const TSIt cbegin() const;

    TSIt end();
    const TSIt cend() const;

    void move();

    void setTimeWindowSize( uint32_t size ) { m_twSize = size; }
    uint32_t timeWindowSize() const { return m_twSize; }

    void setDirection( TSDirection dir ) { m_dir = dir; }
    TSDirection direction() const { return m_dir; }

    TSData& data() { return m_data; }
    const TSData& data() const { return m_data; }

private:
    size_t m_curPos;
    uint32_t m_twSize;
    TSDirection m_dir;
    TSData m_data;
};

} // end namespace mld

#endif // MLD_TIMESERIES_H
