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
    template <typename U> friend class TimeSeries;
public:
    TSIndexIt( TSData<T>& v, std::size_t index ) : m_v(&v), m_index(index) {}

    // if needed.
    typename TSData<T>::iterator getRegularIterator() const { return m_v->begin() + m_index; }

    T& operator *() const { return m_v->at(m_index); }
    T* operator ->() const { return &m_v->at(m_index); }

    TSIndexIt& operator ++() { ++m_index; return *this; }
    TSIndexIt& operator +=( std::ptrdiff_t offset ) { m_index += offset; return *this; }
    TSIndexIt operator +( std::ptrdiff_t offset ) const { TSIndexIt res (*this); res += offset; return res; }

    TSIndexIt& operator --() { --m_index; return *this;}
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

enum class TSDirection {
    PAST,
    FUTURE,
    BOTH
};

template <typename T>
class MLD_API TimeSeries
{
public:
    using TSIt = TSIndexIt<T>;

    TimeSeries();
    TimeSeries( size_t radius );
    TimeSeries( TSDirection dir );
    TimeSeries( size_t radius, TSDirection dir );
    TimeSeries( const TimeSeries<T>& rhs );
    TimeSeries( TimeSeries&& rhs );
    TimeSeries& operator =( TimeSeries<T> rhs );
    virtual ~TimeSeries() {}

    friend void swap( TimeSeries& lhs, TimeSeries& rhs )
    {
        using std::swap;

        swap(lhs.m_radius, rhs.m_radius);
        swap(lhs.m_dir, rhs.m_dir);
        swap(lhs.m_data, rhs.m_data);
        swap(lhs.m_curPos.m_index, rhs.m_curPos.m_index);
        swap(lhs.m_start.m_index, rhs.m_start.m_index);
        swap(lhs.m_finish.m_index, rhs.m_finish.m_index);
    }

    inline TSIt sliceBegin() { return m_start; }
    inline TSIt sliceEnd() { return m_finish; }
    inline TSIt current() { return m_curPos; }

    inline const TSIt csliceBegin() const { return m_start; }
    inline const TSIt csliceEnd() const { return m_finish; }
    inline const TSIt ccurrent() const { return m_curPos; }


    inline TSIt begin() { return TSIndexItBegin(m_data); }
    inline TSIt end() { return TSIndexItEnd(m_data); }

    inline bool empty() const { return m_data.empty(); }

    inline size_t radius() const { return m_radius; }
    /**
     * @brief Set the time window radius for the slices
     * @param newSize
     */
    void setRadius( size_t newSize );

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
    TSIt scroll( int delta = 1 );
    void resetSlice();
    void clear();
    /**
     * @brief Drop values from TSData to only keep those in the slice
     */
    void shrink();

    void push_back( const T& value );
    void push_front( const T& value );
    void pop_back();
    void pop_front();

    /**
     * @brief Adjust slice after the structure of the underlying has been modified
     */
    void clamp();
private:
    void moveIt( TSIt* it, int delta );
    void moveAll( int delta );

private:
    size_t m_radius;
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
    : m_radius(0)
    , m_dir(TSDirection::BOTH)
    , m_data()
    , m_curPos(TSIndexItBegin(m_data))
    , m_start(TSIndexItBegin(m_data))
    , m_finish(TSIndexItEnd(m_data))
{
}

template <typename T>
TimeSeries<T>::TimeSeries( size_t radius )
    : TimeSeries()
{
    m_radius = radius;
}

template <typename T>
TimeSeries<T>::TimeSeries( TSDirection dir )
    : TimeSeries()
{
    m_dir = dir;
}

template <typename T>
TimeSeries<T>::TimeSeries( size_t radius, TSDirection dir )
    : TimeSeries()
{
    m_radius = radius;
    m_dir = dir;
}

template <typename T>
TimeSeries<T>::TimeSeries( const TimeSeries<T>& rhs )
    : m_radius(rhs.m_radius)
    , m_dir(rhs.m_dir)
    , m_data(rhs.m_data)
    , m_curPos(TSIt(m_data, rhs.m_curPos.m_index))
    , m_start(TSIt(m_data, rhs.m_start.m_index))
    , m_finish(TSIt(m_data, rhs.m_finish.m_index))
{
}

template <typename T>
TimeSeries<T>::TimeSeries( TimeSeries&& rhs )
    : TimeSeries()
{
    swap(*this, rhs);
}

template <typename T>
TimeSeries<T>& TimeSeries<T>::operator =( TimeSeries<T> rhs )
{
    swap(*this, rhs);
    return *this;
}

template <typename T>
auto TimeSeries<T>::scroll( int delta ) -> TSIt
{
    moveAll(delta);
    return m_curPos;
}

template <typename T>
void TimeSeries<T>::setRadius( size_t newSize )
{
    if( m_radius == newSize )
        return;
    m_radius = newSize;
    clamp();
}

template <typename T>
void TimeSeries<T>::setDirection( TSDirection dir )
{
    if( m_dir == dir )
        return;
    m_dir = dir;
    clamp();
}


template <typename T>
void TimeSeries<T>::clear()
{
    m_data.clear();
    m_curPos = TSIndexItBegin(m_data);
    clamp();
}

template <typename T>
void TimeSeries<T>::resetSlice()
{
    m_curPos = TSIndexItBegin(m_data);
    clamp();
}

template <typename T>
void TimeSeries<T>::push_back( const T& value )
{
    m_data.push_back(value);
    clamp();
}

template <typename T>
void TimeSeries<T>::push_front( const T& value )
{
    m_data.push_front(value);
    moveAll(1);
}

template <typename T>
void TimeSeries<T>::pop_back()
{
    assert(!m_data.empty());
    m_data.pop_back();

    if( m_curPos == end() )
        moveAll(-1);
    else
        clamp();
}

template <typename T>
void TimeSeries<T>::pop_front()
{
    assert(!m_data.empty());
    m_data.pop_front();
    moveAll(-1);
}

template <typename T>
void TimeSeries<T>::shrink()
{
    size_t startIdx = m_start.index();
    for( size_t i = 0; i < startIdx; ++i )
        m_data.pop_front();

    size_t endDelta = distance(end(), m_finish);
    for( size_t i = 0; i < endDelta; ++i )
        m_data.pop_back();

    clamp();
}

// Private
template <typename T>
void TimeSeries<T>::moveAll( int delta )
{
    moveIt(&m_curPos, delta);
    clamp();
}

template <typename T>
void TimeSeries<T>::clamp()
{
    m_finish = m_curPos;
    m_start = m_curPos;

    moveIt(&m_finish, m_radius + 1);
    moveIt(&m_start, -m_radius);

    // Reset start and finish iterator from cur Pos
    switch( m_dir ) {
        case TSDirection::PAST: {
            m_finish = m_curPos;
            // Finish is always one after curPos if possible
            moveIt(&m_finish, 1);
        }
        break;

        case TSDirection::FUTURE: {
            m_start = m_curPos;
        }
        break;

        case TSDirection::BOTH: {
            ; // All set
        }
        break;

        default:
            LOG(logERROR) << "TimeSeries<T>::clamp Invalid direction";
        break;
    }
}

template <typename T>
void TimeSeries<T>::moveIt( TSIt* it, int delta )
{
    if( delta > 0 ) {
        auto e = end();
        if( (it->index() + delta) > e.index() ) {
            *it = e;
        }
        else {
            *it += delta;
        }
    }
    else {  // delta < 0
        auto b = begin();
        int v = it->index() + delta;
        if( v < int(b.index()) ) {
            *it = b;
        }
        else {
            *it += delta;
        }
    }
}

} // end namespace mld

template <typename T>
std::ostream& operator <<( std::ostream& out, const mld::TSIndexIt<T>& rhs )
{
    out << "index:" << rhs.index() << " ";
    return out;
}

template <typename T>
std::ostream& operator <<( std::ostream& out, const mld::TimeSeries<T>& rhs )
{
    out << "start:{" << rhs.csliceBegin() << "} "
        << "curPos:{" << rhs.ccurrent() << "} "
        << "end:{" << rhs.csliceEnd() << "} "
        << "radius:" << rhs.radius() << " ";

    out << " sliced data:";
    for( auto it = rhs.csliceBegin(); it != rhs.csliceEnd(); ++it )
        out << " " << *it;
    return out;
}

#endif // MLD_TIMESERIES_H
