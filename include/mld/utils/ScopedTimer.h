#ifndef MLD_SCOPEDTIMER_H
#define MLD_SCOPEDTIMER_H

/******************************************************************************/
/*  Copyright (c) 2010-2011, Tim Day <timday@timday.com>                      */
/*                                                                            */
/*  Permission to use, copy, modify, and/or distribute this software for any  */
/*  purpose with or without fee is hereby granted, provided that the above    */
/*  copyright notice and this permission notice appear in all copies.         */
/*                                                                            */
/*  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  */
/*  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF          */
/*  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR   */
/*  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN     */
/*  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF   */
/*  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.            */
/******************************************************************************/

#include <boost/noncopyable.hpp>
#include <chrono>
#include <iostream>

// Utility class for timing and logging rates
// (ie "things-per-second").
// NB _any_ destructor invokation (including early return
// from a function or exception throw) will trigger an
// output which will assume that whatever is being measured
// has completed successfully and fully.

namespace mld {

class ScopedTimer: boost::noncopyable
{
public:
    typedef std::chrono::high_resolution_clock clock_type;

    ScopedTimer( const std::string& what, const std::string& units, double n )
        :_what(what)
        ,_units(units)
        ,_how_many(n)
        ,_start(clock_type::now())
    {
    }

    ~ScopedTimer()
    {
        clock_type::time_point stop = clock_type::now();
        const double t = 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(stop - _start).count();
        std::cout << (boost::format("%1%: %|2$-5.3g| %|3$|/s (%|4$-5.3g|s)")
                      % _what % (_how_many/t) % _units % t)
                  << std::endl;
    }

private:
    const std::string _what;
    const std::string _units;
    const double _how_many;
    const clock_type::time_point _start;
};

} // end namespace mld

#endif // MLD_SCOPEDTIMER_H
