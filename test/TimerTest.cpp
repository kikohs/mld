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

#include <iostream>

#include <mld/utils/Timer.h>
#include <mld/utils/ScopedTimer.h>

using namespace mld;

int main(int ac, char* av[])
{
    std::cout << std::chrono::high_resolution_clock::period::den << std::endl;

    std::unique_ptr<Timer> section_timer(new Timer("Option Parse"));
    // MAGIC: Parse the command line...

    section_timer.reset(new Timer("File Load"));
    // MAGIC: Load a big file...

    section_timer.reset(new Timer("DB Connect"));
    // MAGIC: Establish a DB connection...

    for( int i = 0; i < 100; ++i ) {
      section_timer.reset(new Timer("Entry Parse"));
      // MAGIC: Parse the incoming item
      section_timer.reset(new Timer("Record Update"));
      // MAGIC: Update the database
    }

    section_timer.reset();
    std::cout << Timer::dumpTrials() << std::endl;


    std::unique_ptr<ScopedTimer> t( new ScopedTimer("Test", "operations", 1) );
    // Test function here
}
