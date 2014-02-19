#!/bin/bash
#
#
# This script works for Unix-like systems.
# Execute it to check Sparksee is working properly.
#
#
# Arguments:
# $1 -> Platform: macos64|linux64|linux32
#
#
# For example:
# $ bash build.sh linux64
#

# Check number of arguments
if [ $# -ne 1 ]
then
    echo "Usage: ./build.sh platform"
    echo "where"
    echo "    platform = macos64|linux64|linux32"
else

    PLATFORM=$1        # platform subdirectory
    SPARKSEECPP_DIST_DIR=.. # Sparksee distribution root directory
    CC=c++             # compiler

    rm -fr ${SPARKSEECPP_DIST_DIR}/test/SparkseecppTest.cpp.o 
    rm -fr ${SPARKSEECPP_DIST_DIR}/test/SparkseecppTest

    ${CC} \
        -I${SPARKSEECPP_DIST_DIR}/includes/stlport \
        -I${SPARKSEECPP_DIST_DIR}/includes/sparksee \
        -D_FILE_OFFSET_BITS=64 \
        -o SparkseecppTest.cpp.o \
        -c SparkseecppTest.cpp

    ${CC} \
        SparkseecppTest.cpp.o \
        -o SparkseecppTest \
        -L${SPARKSEECPP_DIST_DIR}/lib/${PLATFORM}/ \
        -lsparksee \
        -lstlport \
        -lpthread

    export DYLD_LIBRARY_PATH=${SPARKSEECPP_DIST_DIR}/lib/${PLATFORM}/    #macos
    export LD_LIBRARY_PATH=${SPARKSEECPP_DIST_DIR}/lib/${PLATFORM}/      #linux

    ./SparkseecppTest

fi



