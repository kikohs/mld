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

TEST( TimeSeriesTest, Ctor )
{
    TimeSeries<int> ts1;
    ts1.setRadius(1);
    ts1.push_back(1);
    ts1.push_back(2);
    ts1.push_back(3);

    // Cpy ctor
    TimeSeries<int> ts2(ts1);
    EXPECT_EQ(ts1.totalSize(), ts2.totalSize());
    EXPECT_EQ(ts1.sliceSize(), ts2.sliceSize());
    EXPECT_EQ(*ts1.sliceBegin(), *ts2.sliceBegin());
    // Should be on the last value since the radius size is 1 and
    // there are 3 values in the object
    EXPECT_EQ(*ts1.sliceEnd(), *ts2.sliceEnd());
    EXPECT_EQ(3, *ts1.sliceEnd());
    EXPECT_EQ(*ts1.current(), *ts2.current());

    // Check ts2 iterators, they should work on their own copy of the data
    ts1.clear();
    EXPECT_TRUE(ts1.empty());
    EXPECT_EQ(1, *ts2.current());

    // Assignement
    TimeSeries<int> ts3 = ts2;
    EXPECT_EQ(ts2.totalSize(), ts3.totalSize());
    EXPECT_EQ(ts2.sliceSize(), ts3.sliceSize());
    EXPECT_EQ(*ts2.sliceBegin(), *ts3.sliceBegin());
    EXPECT_EQ(*ts2.sliceEnd(), *ts3.sliceEnd());
    EXPECT_EQ(*ts2.current(), *ts3.current());

    // Check ts3 iterators, they should work on their own copy of the data
    ts2.clear();
    EXPECT_EQ(1, *ts3.current());

    TimeSeries<int> ts4(3);
    ts4.push_back(1); ts4.push_back(2); ts4.push_back(3);
    ts4.push_back(4); ts4.push_back(5); ts4.push_back(6);

    EXPECT_EQ(ts4.sliceBegin().index() + ts4.radius() + 1, ts4.sliceEnd().index());
    TimeSeries<int> ts5(ts4);
    EXPECT_EQ(ts5.sliceBegin().index() + ts5.radius() + 1, ts5.sliceEnd().index());
}

TEST( TimeSeriesTest, Slices )
{
    TimeSeries<int> ts;

    bool ok = true;
    // Should not pass
    for( auto it = ts.begin(); it != ts.end(); ++it )
        ok = false;
    EXPECT_TRUE(ok);

    for( auto it = ts.sliceBegin(); it != ts.sliceEnd(); ++it )
        ok = false;
    EXPECT_TRUE(ok);

    ts.push_back(1);
    for( auto it = ts.sliceBegin(); it != ts.sliceEnd(); ++it )
        EXPECT_EQ(1, *it);

    EXPECT_EQ(size_t(1), ts.sliceSize());

    // Move forward 1 step
    auto it = ts.scroll();
    EXPECT_EQ(ts.end(), it);
    // Out of bound but controlled
    auto it2 = ts.scroll();
    EXPECT_EQ(ts.end(), it2);

    ts.push_back(2); ts.push_back(3); ts.push_back(4);
    ts.push_back(5); ts.push_back(6);

    ts.setRadius(3);

    // End slice should have moved
    EXPECT_EQ(2, *ts.current());
    // Radius is 3, curPos at 1 so we look 3
    // values ahead + 1 because it is the end iterator
    EXPECT_EQ(6, *ts.sliceEnd());

    // Scroll forwards
    ts.scroll(100);
    EXPECT_EQ(ts.sliceBegin().index(), ts.sliceEnd().index() - 3);
    EXPECT_EQ(ts.sliceBegin().index(), ts.current().index() - 3);

    // Scroll backwards
    ts.scroll(-100);
    EXPECT_EQ(ts.sliceBegin(), ts.current());
    EXPECT_EQ(ts.current() + ts.radius() + 1, ts.sliceEnd());

    // Scroll forward to test reset
    ts.scroll(100);
    ts.resetSlice();
    EXPECT_EQ(ts.sliceBegin(), ts.current());
    EXPECT_EQ(ts.current() + ts.radius() + 1, ts.sliceEnd());
}

TEST( TimeSeriesTest, pop )
{
    TimeSeries<int> ts(3);
    ts.push_back(1); ts.push_back(2); ts.push_back(3);
    ts.push_back(4); ts.push_back(5); ts.push_back(6);

    TimeSeries<int> ts2(ts);

    // 6 values before
    EXPECT_EQ(size_t(6), ts.totalSize());
    EXPECT_EQ(size_t(4), ts.sliceSize());
    // Pop back
    ts.pop_back(); ts.pop_back(); ts.pop_back();

    EXPECT_EQ(size_t(3), ts.totalSize());
    EXPECT_EQ(size_t(3), ts.sliceSize());
    // No changes the slice end is before the real end
    EXPECT_EQ(ts.sliceBegin(), ts.current());
    EXPECT_EQ(ts.sliceEnd(), ts.end());

    ts.pop_back(); ts.pop_back(); ts.pop_back();
    EXPECT_EQ(size_t(0), ts.totalSize());
    EXPECT_EQ(size_t(0), ts.sliceSize());
    EXPECT_EQ(ts.sliceBegin(), ts.current());
    EXPECT_EQ(ts.sliceEnd(), ts.end());
    EXPECT_EQ(ts.sliceEnd(), ts.sliceBegin());
    EXPECT_EQ(ts.sliceBegin(), ts.begin());

    // Pop front
    ts2.pop_front();
    EXPECT_EQ(size_t(5), ts2.totalSize());
    EXPECT_EQ(size_t(4), ts2.sliceSize());
    EXPECT_EQ(ts2.sliceBegin(), ts2.current());
    EXPECT_EQ(ts2.sliceBegin(), ts2.begin());
    EXPECT_EQ(ts2.current().index() + ts2.radius() + 1, ts2.sliceEnd().index());

    ts2.push_front(1);
    EXPECT_EQ(size_t(6), ts2.totalSize());
    EXPECT_EQ(size_t(5), ts2.sliceSize());
    EXPECT_EQ(ts2.current().index(), ts2.sliceBegin().index() +1);
    EXPECT_EQ(ts2.sliceBegin(), ts2.begin());
    EXPECT_EQ(ts2.current().index() + ts2.radius() + 1, ts2.sliceEnd().index());

}
