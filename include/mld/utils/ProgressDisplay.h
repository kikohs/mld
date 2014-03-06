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

#ifndef MLD_PROGRESSDISPLAY_H
#define MLD_PROGRESSDISPLAY_H

#include <boost/noncopyable.hpp>

namespace mld {

// Inspired by <boost/progress.hpp>

class ProgressDisplay: private boost::noncopyable
{
public:
    explicit ProgressDisplay( unsigned long expected_count,
                               std::ostream & os = std::clog,
                               const std::string & s1 = "\n", //leading strings
                               const std::string & s2 = "",
                               const std::string & s3 = "" )
        : noncopyable()
        , m_os(os)
        , m_s1(s1)
        , m_s2(s2)
        , m_s3(s3)
    {
        restart(expected_count);
    }

    void restart( unsigned long expected_count )
    {
        //  Effects: display appropriate scale
        //  Postconditions: count()==0, expected_count()==expected_count
        m_count = m_next_tic_count = m_tic = 0;
        m_expected_count = expected_count;

        m_os << m_s1 << "0%   10   20   30   40   50   60   70   80   90   100%\n"
             << m_s2 << "|----|----|----|----|----|----|----|----|----|----|"
             << std::endl  // endl implies flush, which ensures display
             << m_s3;
        if( !m_expected_count )
            m_expected_count = 1;  // prevent divide by zero
    }

    unsigned long operator+=( unsigned long increment )
    {
        //  Effects: Display appropriate progress tic if needed.
        //  Postconditions: count()== original count() + increment
        //  Returns: count().
        if( (m_count += increment) >= m_next_tic_count ) {
            displayTic();
        }
        return m_count;
    }

    unsigned long operator++()           { return operator+=( 1 ); }
    unsigned long count() const          { return m_count; }
    unsigned long expectedCount() const  { return m_expected_count; }

private:
    void displayTic()
    {
        int barWidth = 49;
        unsigned int tics_needed =
                static_cast<unsigned int>(
                    (static_cast<double>(m_count) / m_expected_count) * barWidth );
        do {
            m_os << std::flush;
        } while ( ++m_tic < tics_needed );

        float progress = static_cast<float>(m_count) / m_expected_count;
        m_os << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) m_os << "=";
            else if (i == pos) m_os << ">";
            else m_os << " ";
        }
        m_os << "] " << static_cast<unsigned int>(progress * 100.0) << " %\r";

        m_next_tic_count = static_cast<unsigned long>( (m_tic / float(barWidth) ) * m_expected_count);
        if ( m_count == m_expected_count )
            m_os << std::endl;
    }

private:
    std::ostream&      m_os;  // may not be present in all imps
    const std::string  m_s1;  // string is more general, safer than
    const std::string  m_s2;  //  const char *, and efficiency or size are
    const std::string  m_s3;  //  not issues

    unsigned long m_count, m_expected_count, m_next_tic_count;
    unsigned int  m_tic;
};

} // end namespace mld

#endif // MLD_PROGRESSDISPLAY_H
