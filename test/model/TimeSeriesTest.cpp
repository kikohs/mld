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

#include <gtest/gtest.h>
#include <mld/model/TimeSeries.h>

using namespace mld;

TEST( TimeSeriesTest, Empty )
{
    TimeSeries<double> ts;

    bool ok = true;
    // Should not pass
    for( auto it = ts.begin(); it != ts.end(); ++it )
        ok = false;
    EXPECT_TRUE(ok);

    for( auto it = ts.sliceBegin(); it != ts.sliceEnd(); ++it )
        ok = false;
    EXPECT_TRUE(ok);

    ts.push_back(12.0);
    for( auto it = ts.sliceBegin(); it != ts.sliceEnd(); ++it )
        EXPECT_DOUBLE_EQ(12.0, *it);

    EXPECT_EQ(size_t(1), ts.sliceSize());

    // Move forward 1 step
    auto it = ts.move();
    EXPECT_EQ(ts.end(), it);
    // Out of bound but controlled
    auto it2 = ts.move();
    // curPos is at end
    EXPECT_EQ(ts.end(), it2);
}
