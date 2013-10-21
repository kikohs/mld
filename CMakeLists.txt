# Kirell Benzi. Copyright (C) 2013. See LICENSE.

#
# Project Properties
#
cmake_minimum_required( VERSION 2.8.8 )

project( mld )

# VERSION
set( ${PROJECT_NAME}_MAJOR_VERSION "0" )
set( ${PROJECT_NAME}_MINOR_VERSION "1" )
set( ${PROJECT_NAME}_PATCH_VERSION "0" )
set( ${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION}" )
set( PROJECT_VERSION ${${PROJECT_NAME}_VERSION} )
set( PROJECT_SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION} )

# Always include the source and build directories in the include path
# to save doing so manually in every subdirectory.
set( CMAKE_INCLUDE_CURRENT_DIR ON )

#
# Debugging Options
#
set( CMAKE_VERBOSE_MAKEFILE 1 ) # Use 1 for debugging, 0 for release

#
# PROJECT OUTPUT PATHS
#
set( LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set( EXEC_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "Installation prefix for executables and object code libraries" FORCE )
set( BIN_INSTALL_DIR ${EXEC_INSTALL_PREFIX}/bin CACHE PATH "Installation prefix for user executables" FORCE )
set( LIB_INSTALL_DIR ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX} CACHE PATH  "Installation prefix for object code libraries" FORCE )
set( INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include CACHE PATH "Installation prefix for C++ header files" FORCE )
set( DOC_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/doc CACHE PATH "Installation prefix for documentation" FORCE )

set( MAINFOLDER ${PROJECT_SOURCE_DIR} )
set( EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin" )
set( LIBRARY_OUTPUT_PATH "${MAINFOLDER}/lib" )
# 3rd Party dir
set( 3RDPARTY_DIR "${MAINFOLDER}/3rdparty" )
# CMAKE_MODULE_PATH
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${MAINFOLDER}/share/cmake" )
# Check of resources folder exists, if not create it
set( RESOURCES_DIR "${MAINFOLDER}/resources/" )
if( NOT EXISTS ${RESOURCES_DIR} )
    file( MAKE_DIRECTORY ${RESOURCES_DIR} )
endif()
# Public header dir
set( PUBLIC_HDRS_DIR ${MAINFOLDER}/include/${PROJECT_NAME} )

#
# LOCAL PREREQUISITES
#

## Boost
set( Boost_USE_STATIC_LIBS ON )
set( Boost_USE_MULTITHREADED ON )
find_package( Boost REQUIRED )
# find_package( Boost COMPONENTS thread filesystem system graph serialization REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )

## Dex
if( NOT DEX_ROOT_DIR )
    if( EXISTS ${3RDPARTY_DIR}/dex/ )
        set( DEX_ROOT_DIR ${3RDPARTY_DIR}/dex CACHE PATH "Location of Dex root folder" )
    else()
        message( FATAL " Unable to find Dex root directory" )
    endif()
endif()

add_definitions( -D_FILE_OFFSET_BITS=64 )
set( DEX_INCLUDE ${DEX_ROOT_DIR}/includes/ )
set( DEX_INCLUDE_DIR ${DEX_INCLUDE} )
# Dex includes are not prefixed by dex/, we need to include /dex ourself ...
include_directories( ${DEX_INCLUDE_DIR} ${DEX_INCLUDE_DIR}/dex )

if( APPLE )
    set( DEX_PLATEFORM macos64 )
elseif( WIN32 )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( DEX_PLATEFORM windows64 )
  else()
    set( DEX_PLATEFORM windows32 )
  endif()
elseif( UNIX )
    set( DEX_PLATEFORM linux64 )
else()
    message( FATAL "DEX doesn't support this architecture" )
endif()
set( DEX_LIB_DIR ${DEX_ROOT_DIR}/lib/${DEX_PLATEFORM} )
# message( STATUS "DEX LIB DIR " ${DEX_LIB_DIR} )
link_directories( ${DEX_LIB_DIR} )
set( DEX_LIBRARIES -ldex )

# Grab libraries to copy them for export
set( DEX_LIB_FILES )
file( GLOB DEX_LIB_FILES ${DEX_LIB_DIR}/*.* )
# Copy dex libs to /lib because install name id is @loader_path
file( COPY ${DEX_LIB_FILES} DESTINATION ${LIBRARY_OUTPUT_PATH} )

#
# PROJECT SETTINGS
#
message( STATUS "Using ${CMAKE_CXX_COMPILER_ID}" )
add_definitions( -Wall -Wextra -Wno-unused-parameter -Wdeprecated-declarations )
# C++11
add_definitions(-std=c++11) #-std=c++11 std=gnu++11
if( APPLE )
    add_definitions(-stdlib=libc++) #stdlib=libstdc++ for gcc 4.2.1 includes ..
    set( LIBCPLUSPLUS "-stdlib=libc++" )
endif()

if( BUILD_SHARED_LIBS )
    add_definitions( -fvisibility=hidden -fvisibility-inlines-hidden )
endif()

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    add_definitions( -O3 )
elseif( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    add_definitions( -DMLD_DEBUG )
endif()

#### SRC && CUSTOM DEFINITIONS
# Config file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/mld/config.h )

#### OPTIONS
option( BUILD_TESTS "Build the test suite" ON )
option( BUILD_TOOLS "Build standard tools" ON )
option( BUILD_DOC "Build documentation" ON )

# SOURCES
add_subdirectory( src )

# TOOLS
if( BUILD_TOOLS )
    add_subdirectory( tools )
endif( BUILD_TOOLS )

# TESTS
if( BUILD_TESTS )
    add_subdirectory( test )
endif( BUILD_TESTS )

# DOC
if( BUILD_DOC )
    # add a target to generate API documentation with Doxygen
    find_package( Doxygen )
    if( DOXYGEN_FOUND )
        configure_file( ${PROJECT_SOURCE_DIR}/src/Doxyfile.in ${PROJECT_SOURCE_DIR}/docs/Doxyfile @ONLY )
        add_custom_target( doc
            ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/Doxyfile
            COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
        file( GLOB doc "${CMAKE_CURRENT_BINARY_DIR}/doc/*.*" )
        install( FILES ${doc} DESTINATION ${DOC_INSTALL_DIR} )
    else()
        add_custom_target( doc COMMENT "Doxygen not found. Please install doxygen to use this target." VERBATIM )
    endif( DOXYGEN_FOUND )
endif( BUILD_DOC )


#
# Add Install Targets for public headers
#
if( EXISTS ${PUBLIC_HDRS_DIR} AND IS_DIRECTORY ${PUBLIC_HDRS_DIR} )
    install( DIRECTORY ${PUBLIC_HDRS_DIR} DESTINATION ${INCLUDE_INSTALL_DIR} )
endif()




