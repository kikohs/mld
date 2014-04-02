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
template <typename T, typename... Ts> using TSData = std::deque<T, Ts...>;

template <typename T, typename... Ts>
class TSIndexIt : public std::iterator<std::random_access_iterator_tag, T>
{
public:
    TSIndexIt( TSData<T, Ts...>& v, std::size_t index ) : v(&v), index(index) {}

    // if needed.
    typename TSData<T, Ts...>::iterator getRegularIterator() const { return v->begin() + index; }

    T& operator *() const { return v->at(index); }
    T* operator ->() const { return &v->at(index); }

    TSIndexIt& operator ++() { ++index; return *this;}
    TSIndexIt& operator ++( int ) { TSIndexIt old(*this); ++*this; return old;}
    TSIndexIt& operator +=( std::ptrdiff_t offset ) { index += offset; return *this;}
    TSIndexIt operator +( std::ptrdiff_t offset ) const { TSIndexIt res (*this); res += offset; return res;}

    TSIndexIt& operator --() { --index; return *this;}
    TSIndexIt& operator --(int) { TSIndexIt old(*this); --*this; return old;}
    TSIndexIt& operator -=( std::ptrdiff_t offset ) { index -= offset; return *this;}
    TSIndexIt operator -( std::ptrdiff_t offset ) const { TSIndexIt res (*this); res -= offset; return res;}

    std::ptrdiff_t operator -( const TSIndexIt& rhs ) const { assert(v == rhs.v); return index - rhs.index; }

    bool operator == ( const TSIndexIt& rhs ) const { assert(v == rhs.v); return index == rhs.index; }
    bool operator != ( const TSIndexIt& rhs ) const { return !(*this == rhs); }

private:
    TSData<T, Ts...>* v;
    std::size_t index;
};

template <typename T, typename... Ts>
TSIndexIt<T, Ts...> IndexIteratorBegin(TSData<T, Ts...>& v)
{
    return TSIndexIt<T, Ts...>(v, 0);
}

template <typename T, typename... Ts>
TSIndexIt<T, Ts...> IndexIteratorEnd(TSData<T, Ts...>& v)
{
    return TSIndexIt<T, Ts...>(v, v.size());
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

    inline TSIt begin() { return m_start; }
    inline TSIt end() { return m_end; }

    TSIt move( int delta = 1 );
    void setTimeWindowSize( uint32_t newSize );

    inline uint32_t timeWindowSize() const { return m_twSize; }

    inline void setDirection( TSDirection dir ) { m_dir = dir; }
    inline TSDirection direction() const { return m_dir; }

    inline TSData<T>& data() { return m_data; }
    inline const TSData<T>& data() const { return m_data; }

    inline void push_back( const T& value ) { m_data.push_back(value); }
    inline void push_front( const T& value ) { m_data.push_front(value); }
    inline void pop_back() { m_data.pop_back(); }
    inline void pop_front() { m_data.pop_front(); }

private:
    void moveIt( TSIt* it, int delta );

private:
    uint32_t m_twSize;
    TSDirection m_dir;
    TSData<T> m_data;

    // Iterators to move in TSData
    TSIt m_curPos;
    TSIt m_start;
    TSIt m_end;
};


/*********** IMPLEMENTATION ***********/

template <typename T>
TimeSeries<T>::TimeSeries()
    : m_twSize(0)
    , m_dir(TSDirection::BOTH)
    , m_data()
    , m_curPos(IndexIteratorBegin(m_data))
    , m_start(IndexIteratorBegin(m_data))
    , m_end(IndexIteratorEnd(m_data))
{
}


template <typename T>
auto TimeSeries<T>::move( int delta ) -> TSIt
{
    moveIt(&m_start, delta);
    moveIt(&m_end, delta);
    moveIt(&m_curPos, delta);
    return m_curPos;
}

template <typename T>
void TimeSeries<T>::setTimeWindowSize( uint32_t newSize )
{
    if( newSize == m_twSize )
        return;

    int delta = newSize - m_twSize;
    m_twSize = newSize;

    if( m_dir == TSDirection::FUTURE || m_dir == TSDirection::BOTH )
        moveIt(&m_end, delta);
    if( m_dir == TSDirection::PAST || m_dir == TSDirection::BOTH )
        moveIt(&m_start, -delta);
}

// Private
template <typename T>
void TimeSeries<T>::moveIt( TSIt* it, int delta )
{
    ; // TODO
}

} // end namespace mld

#endif // MLD_TIMESERIES_H
