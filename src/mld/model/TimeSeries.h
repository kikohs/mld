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

#include <cstddef>
#include <deque>

#include "mld/common.h"

namespace mld {

/**
 * Stable iterator for TimeSeries data even if data is
 * added in the underlying container via push_front or push_back
 */
// Container used for TSData is a std::deque
template <typename T> using TSData = std::deque<T>;

template <typename T>
class TSIndexIt : public std::iterator<std::random_access_iterator_tag, T>
{
    template <typename U> friend size_t distance( const TSIndexIt<U>& lhs, const TSIndexIt<U>& rhs );
public:
    TSIndexIt( TSData<T>& v, std::size_t index ) : m_v(&v), m_index(index) {}

    // if needed.
    typename TSData<T>::iterator getRegularIterator() const { return m_v->begin() + m_index; }

    T& operator *() const { return m_v->at(m_index); }
    T* operator ->() const { return &m_v->at(m_index); }

    TSIndexIt& operator ++() { ++m_index; return *this; }
    TSIndexIt& operator ++( int ) { TSIndexIt old(*this); ++*this; return old; }
    TSIndexIt& operator +=( std::ptrdiff_t offset ) { m_index += offset; return *this; }
    TSIndexIt operator +( std::ptrdiff_t offset ) const { TSIndexIt res (*this); res += offset; return res; }

    TSIndexIt& operator --() { --m_index; return *this;}
    TSIndexIt& operator --(int) { TSIndexIt old(*this); --*this; return old; }
    TSIndexIt& operator -=( std::ptrdiff_t offset ) { m_index -= offset; return *this; }
    TSIndexIt operator -( std::ptrdiff_t offset ) const { TSIndexIt res (*this); res -= offset; return res; }

    std::ptrdiff_t operator -( const TSIndexIt& rhs ) const { assert(m_v == rhs.m_v); return m_index - rhs.m_index; }

    bool operator == ( const TSIndexIt& rhs ) const { assert(m_v == rhs.m_v); return m_index == rhs.m_index; }
    bool operator != ( const TSIndexIt& rhs ) const { return !(*this == rhs); }

    bool operator <( const TSIndexIt& rhs ) const { assert(m_v == rhs.m_v); return m_index < rhs.m_index; }
    bool operator >( const TSIndexIt& rhs ) const { assert(m_v == rhs.m_v); return m_index > rhs.m_index; }
    bool operator >=( const TSIndexIt& rhs ) const { assert(m_v == rhs.m_v); return m_index >= rhs.m_index; }
    bool operator <=( const TSIndexIt& rhs ) const { assert(m_v == rhs.m_v); return m_index <= rhs.m_index; }

    size_t index() const { return m_index; }
private:
    TSData<T>* m_v;
    size_t m_index;
};

template <typename T>
size_t distance( const TSIndexIt<T>& lhs, const TSIndexIt<T>& rhs )
{
    if( lhs > rhs )
        return lhs.m_index - rhs.m_index;
    return rhs.m_index < lhs.m_index;
}

template <typename T>
TSIndexIt<T> TSIndexItBegin( TSData<T>& v )
{
    return TSIndexIt<T>(v, 0);
}

template <typename T>
TSIndexIt<T> TSIndexItEnd( TSData<T>& v )
{
    return TSIndexIt<T>(v, v.size());
}

template <typename T>
class MLD_API TimeSeries
{
public:
    using TSIt = TSIndexIt<T>;

    enum class TSDirection {
        PAST,
        FUTURE,
        BOTH
    };

    TimeSeries();
    virtual ~TimeSeries() {}

    inline TSIt sliceBegin() { return m_start; }
    inline TSIt sliceEnd() { return m_finish; }
    inline TSIt current() { return m_curPos; }

    inline TSIt begin() { return TSIndexItBegin(m_data); }
    inline TSIt end() { return TSIndexItEnd(m_data); }

    inline bool empty() const { return m_data.empty(); }

    inline size_t timeWindowSize() const { return m_twSize; }
    void setTimeWindowSize( size_t newSize );

    inline TSData<T>& data() { return m_data; }
    inline const TSData<T>& data() const { return m_data; }

    inline TSDirection direction() const { return m_dir; }
    void setDirection( TSDirection dir );

    inline size_t totalSize() const { return m_data.size(); }
    inline size_t sliceSize() const { return distance<T>(m_finish, m_start); }

    /**
     * @brief Move the slice forward from 1 step
     * @param delta
     * @return curPos iterator
     */
    TSIt move( int delta = 1 );
    void resetSlice();
    void clear();

    void push_back( const T& value );
    void push_front( const T& value );
    void pop_back();
    void pop_front();

private:
    void moveIt( TSIt* it, int delta );

private:
    size_t m_twSize;
    TSDirection m_dir;
    TSData<T> m_data;

    // Iterators to move in TSData
    TSIt m_curPos;
    TSIt m_start;
    TSIt m_finish;
};


/*********** IMPLEMENTATION ***********/

template <typename T>
TimeSeries<T>::TimeSeries()
    : m_twSize(0)
    , m_dir(TSDirection::BOTH)
    , m_data()
    , m_curPos(TSIndexItBegin(m_data))
    , m_start(TSIndexItBegin(m_data))
    , m_finish(TSIndexItEnd(m_data))
{
}


template <typename T>
auto TimeSeries<T>::move( int delta ) -> TSIt
{
    moveIt(&m_start, delta);
    moveIt(&m_finish, delta);
    moveIt(&m_curPos, delta);

    assert(m_start <= m_curPos && m_curPos <= m_finish);

    return m_curPos;
}

template <typename T>
void TimeSeries<T>::setTimeWindowSize( size_t newSize )
{
    if( newSize == m_twSize )
        return;

    int delta = int(newSize) - int(m_twSize);
    m_twSize = newSize;

    if( m_dir == TSDirection::FUTURE || m_dir == TSDirection::BOTH )
        moveIt(&m_finish, delta);
    if( m_dir == TSDirection::PAST || m_dir == TSDirection::BOTH )
        moveIt(&m_start, -delta);

    assert(m_start <= m_curPos && m_curPos <= m_finish);
}

template <typename T>
void TimeSeries<T>::setDirection( TSDirection dir )
{
    if( m_dir == dir )
        return;

    switch( m_dir ) {
        case TSDirection::PAST:
            if( dir == TSDirection::FUTURE ) {
                m_start = TSIt(m_curPos);
            }
            // BOTH moves the end cursor
            moveIt(&m_finish, m_twSize);
            break;

        case TSDirection::FUTURE:
            if( dir == TSDirection::PAST ) {
                m_finish = TSIt(m_curPos);
            }
            // BOTH moves the start cursor backwards
            moveIt(&m_start, -m_twSize);
            break;

        case TSDirection::BOTH:
            if( dir == TSDirection::FUTURE ) {
                m_start = TSIt(m_curPos);
            } else {  // PAST
                m_finish = TSIt(m_curPos);
            }
            break;

        default:
            LOG(logERROR) << "TimeSeries<T>::setDirection Invalid direction";
            return;
    }

    assert(m_start <= m_curPos && m_curPos <= m_finish);
    // Update direction
    m_dir = dir;
}


template <typename T>
void TimeSeries<T>::clear()
{
    m_data.clear();
    m_curPos = TSIndexItBegin(m_data);
    m_start = TSIndexItBegin(m_data);
    m_finish = TSIndexItEnd(m_data);
}

template <typename T>
void TimeSeries<T>::resetSlice()
{
    m_curPos = TSIndexItBegin(m_data);
    m_start = TSIndexItBegin(m_data);
    m_finish = TSIndexItBegin(m_data);
    if( m_dir != TSDirection::PAST ) {
        moveIt(&m_finish, m_twSize);
    } else {  // PAST
        // Set the end to be curPos + 1
        m_finish++;
    }
}

template <typename T>
void TimeSeries<T>::push_back( const T& value )
{
    assert(m_start <= m_curPos && m_curPos <= m_finish);
    m_data.push_back(value);
    if( m_dir != TSDirection::PAST ) {
        if( distance<T>(m_finish, m_curPos) <= m_twSize ) {
            ++m_finish;
        }
    }
}

template <typename T>
void TimeSeries<T>::push_front( const T& value )
{
    assert(m_start <= m_curPos && m_curPos <= m_finish);
    m_data.push_front(value);
    // Finish and curPoint point to previous index, we should inc them
    ++m_finish;
    ++m_curPos;

    if( m_dir == TSDirection::FUTURE ) {
        ++m_start;
    } else {  // PAST or BOTH
        // Move start bound only if the radius bound is reached
        if( distance<T>(m_curPos, m_start) > m_twSize ) {
            ++m_start;
        }
    }
}

template <typename T>
void TimeSeries<T>::pop_back()
{
    assert(!m_data.empty());
    assert(m_start <= m_curPos && m_curPos <= m_finish);

    if( m_finish == end() )
        --m_finish;

    // Dec curPos it was on the data's last value
    if( m_curPos == m_finish )
        --m_curPos;

    // If start is collapsed on curPos at the end of the slice
    if( m_start == m_finish )
        --m_start;

    m_data.pop_back();
}

template <typename T>
void TimeSeries<T>::pop_front()
{
    assert(!m_data.empty());
    assert(m_start <= m_curPos && m_curPos <= m_finish);

    --m_finish;
    --m_curPos;
    --m_start;

    m_data.pop_front();
}

// Private
template <typename T>
void TimeSeries<T>::moveIt( TSIt* it, int delta )
{
    *it += delta;
    auto b = begin();
    auto e = end();
    if( *it > e ) {
        *it = e;
    }
    else if( *it < b ) {
        *it = b;
    }
    else {
        ; // do nothing
    }
}

} // end namespace mld

#endif // MLD_TIMESERIES_H
