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

#ifndef MLD_TIMER_H
#define MLD_TIMER_H

#include <iostream>
#include <iomanip>      // std::setw
#include <map>
#include <vector>
#include <set>
#include <numeric>      // std::accumulate

#include <boost/format.hpp>
#include <chrono>
#include <math.h>

namespace mld {

template <typename InputIterator, typename OutputIterator,
          typename UnaryPredicate, typename UnaryOperation>

OutputIterator transform_if( InputIterator first, InputIterator last,
                             OutputIterator result,
                             UnaryPredicate unary_predicate,
                             UnaryOperation unary_operation )
{
   for ( ; first != last ; ++first )
   {
      if ( unary_predicate( *first ) ) *result++ = unary_operation( *first );
   }
   return ( result );
}


class Timer
{
public:
    typedef std::chrono::high_resolution_clock clock_type;

    inline Timer( const std::string& name );
    inline Timer( const Timer& rhs );
    inline virtual ~Timer();
    inline std::string format_now() const;
    double current_elapsed() const;

    Timer& operator=( const std::string& name );

    static std::string dumpTrials();
    static void reset();
    static std::string format_elapsed( double d );

private:
    typedef std::string key_type;
    typedef double time_type;
    typedef std::multimap<key_type, time_type> times;

    static int64_t TimerFreq();

    clock_type::time_point m_startTime, m_stopTime;
    mutable clock_type::time_point m_intermediateTime;
    std::string m_label;

    static times& TimeMap();

    struct extract_key : public std::unary_function<times::value_type, key_type>
    {
        std::string operator()(times::value_type const & r) const
        {
            return r.first;
        }
    };

    struct extract_val : public std::unary_function<times::value_type, time_type>
    {
        time_type operator()(times::value_type const & r) const
        {
            return r.second;
        }
    };
    struct match_key : public std::unary_function<times::value_type, bool>
    {
        match_key(key_type const & key_) : key(key_) {}
        bool operator()(times::value_type const & rhs) const
        {
            return key == rhs.first;
        }
        match_key& operator=(const match_key& rhs)
        {
            key = rhs.key;
            return * this;
        }
    protected:
        key_type key;
    };

    struct accum_key : public std::binary_function<time_type, times::value_type, time_type>
    {
        accum_key(key_type const & key_) : key(key_), n(0) {}
        time_type operator()(time_type const & v, times::value_type const & rhs) const
        {
            if( key == rhs.first )
            {
                ++n;
                return rhs.second + v;
            }
            return v;
        }
    private:
        times::key_type key;
        mutable size_t n;
    };
};

inline Timer::Timer(const std::string & name)
{
    m_label = name;
    m_startTime = clock_type::now();
}

inline double Timer::current_elapsed() const
{
    m_intermediateTime = clock_type::now();
    const double elapsed = 1e-9*std::chrono::duration_cast<std::chrono::nanoseconds>(m_intermediateTime-m_startTime).count();
    return elapsed;
}

inline Timer::~Timer()
{
    double elapsed = current_elapsed();
    m_stopTime = m_intermediateTime;
    TimeMap().insert(std::make_pair(m_label, elapsed));
}

inline Timer& Timer::operator=(const std::string& name)
{
    double elapsed = current_elapsed();
    m_stopTime = m_intermediateTime;
    TimeMap().insert(std::make_pair(m_label,elapsed));
    m_label = name;
    m_startTime = clock_type::now();
    return * this;
}

inline Timer::Timer(const Timer& rhs)
{
    double elapsed = current_elapsed();
    m_stopTime = m_intermediateTime;
    TimeMap().insert(std::make_pair(m_label,elapsed));
    m_label = rhs.m_label;
    m_startTime = clock_type::now();
}

inline std::string Timer::format_now() const
{
    return format_elapsed(current_elapsed());
}

inline std::string Timer::dumpTrials()
{
    using boost::io::group;

    if( TimeMap().empty() ) {
        return "No trials\r\n";
    }

    std::string ret = (boost::format("\r\n\r\nTimer Precision = %s\r\n\r\n")
                       % format_elapsed(1.0/(double)TimerFreq())).str();

    // get a list of keys
    typedef std::set<std::string> keyset;
    keyset keys;
    std::transform(TimeMap().begin(), TimeMap().end(), std::inserter(keys, keys.begin()), extract_key());

    size_t maxrows = 0;

    typedef std::vector<std::string> strings;
    static const size_t t = 9;

    std::string head =
            (boost::format("=== %-s %-s %-s %-s %-s %-s ===")
             //% (t*2)
             //% (t*2)
             % group(std::setw(t*5), std::setprecision(t*2), "Item" )
             //% t
             //% t
             % group(std::setw(t), std::setprecision(t), "Trials")
             //% t
             //% t
             % group(std::setw(t), std::setprecision(t),  "Ttl Time" )
             //% t
             //% t
             % group(std::setw(t), std::setprecision(t), "Avg Time" )
             //% t
             //% t
             % group(std::setw(t), std::setprecision(t),  "Mean Time" )
             //% t
             //% t
             % group(std::setw(t), std::setprecision(t),  "StdDev")
             ).str();

    ret += (boost::format("\r\n%s\r\n") % head).str();
    // dump the values for each key
    for( keyset::iterator key = keys.begin(); keys.end() != key; ++key ) {
        time_type ttl = 0;
        ttl = std::accumulate(TimeMap().begin(), TimeMap().end(), ttl, accum_key(*key));
        size_t num = std::count_if( TimeMap().begin(), TimeMap().end(), match_key(*key));
        if( num > maxrows )
            maxrows = num;
        time_type avg = ttl / num;

        // compute mean
        std::vector<time_type> sortedTimes;
        transform_if(TimeMap().begin(), TimeMap().end(), std::inserter(sortedTimes, sortedTimes.begin()), extract_val(), match_key(*key));
        std::sort(sortedTimes.begin(), sortedTimes.end());
        size_t mid = (size_t)floor((double)num/2.0);
        double mean = ( num > 1 && (num % 2) != 0 ) ? (sortedTimes[mid]+sortedTimes[mid+1])/2.0 : sortedTimes[mid];

        // compute variance
        double sum = 0.0;
        if( num > 1 ) {
            for( std::vector<time_type>::iterator cur = sortedTimes.begin(); sortedTimes.end() != cur; ++cur )
                sum += pow(*cur-mean,2.0);
        }

        // compute std dev
        double stddev = num > 1 ? sqrt(sum/((double)num-1.0)) : 0.0;

        ret += (boost::format("    %-s %-s %-s %-s %-s %-s\r\n")
                % group(std::setw(t*5), std::setprecision(t*2) , (*key))
                % group(std::setw(t), std::setprecision(t) , (boost::format("%d") %num).str() )
                % group(std::setw(t), std::setprecision(t) , format_elapsed(ttl).c_str() )
                % group(std::setw(t), std::setprecision(t) , format_elapsed(avg) )
                % group(std::setw(t), std::setprecision(t) , format_elapsed(mean) )
                % group(std::setw(t), std::setprecision(t) , format_elapsed(stddev)) ).str();
    }
    ret += (boost::format("%s\r\n")
            % std::string(head.length(),'=') ).str();

    return ret;
}

inline std::string Timer::format_elapsed(double d)
{
    if( d < 0.00000001 )
    {
        // show in ps with 4 digits
        return (boost::format("%0.4f ps") % (d * 1000000000000.0)).str();
    }
    if( d < 0.00001 )
    {
        // show in ns
        return (boost::format("%0.0f ns")% (d * 1000000000.0)).str();
    }
    if( d < 0.001 )
    {
        // show in us
        return (boost::format("%0.0f us") % (d * 1000000.0)).str();
    }
    if( d < 0.1 )
    {
        // show in ms
        return (boost::format("%0.0f ms") % (d * 1000.0)).str();
    }
    if( d <= 60.0 )
    {
        // show in seconds
        return (boost::format("%0.2f s") % d).str();
    }
    if( d < 3600.0 )
    {
        // show in min:sec
        return (boost::format("%01.0f:%02.2f") % floor(d/60.0) % fmod(d,60.0)).str();
    }
    // show in h:min:sec
    return (boost::format("%01.0f:%02.0f:%02.2f") % floor(d/3600.0) % floor(fmod(d,3600.0)/60.0) % fmod(d,60.0)).str();
}

inline void Timer::reset()
{
    TimeMap().clear();
}

inline int64_t Timer::TimerFreq()
{
    static int64_t freq = 0;
    static bool init = false;
    if( !init ) {
        freq = std::chrono::high_resolution_clock::period::den;
        init = true;
    }
    return freq;
}

inline Timer::times& Timer::TimeMap()
{
    static times times_;
    return times_;
}

} // end namespace mld

#endif // MLD_TIMER_H
