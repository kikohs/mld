# Kirell Benzi. Copyright (C) 2013. See LICENSE.

#
# Project Properties
#
cmake_minimum_required( VERSION 2.8.12 )

project( mld )

# VERSION
set( ${PROJECT_NAME}_MAJOR_VERSION "0" )
set( ${PROJECT_NAME}_MINOR_VERSION "1" )
set( ${PROJECT_NAME}_PATCH_VERSION "0" )
set( ${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION}" )
set( PROJECT_VERSION ${${PROJECT_NAME}_VERSION} )
set( PROJECT_SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION} )

#
# OPTIONS
#
option( BUILD_TESTS "Build the test suite" ON )
option( BUILD_TOOLS "Build standard tools" ON )
option( BUILD_DOC "Build documentation" ON )
option( SAFE_CHECKS "Enable safety checks for the project" ON )
option( USE_JSON "Use json export on the project" ON )


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
set( RESOURCES_DIR "${MAINFOLDER}/resources" )
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

## SPARKSEE
if( NOT SPARKSEE_ROOT_DIR )
    if( EXISTS ${3RDPARTY_DIR}/sparkseecpp-5.0.0/ )
        set( SPARKSEE_ROOT_DIR ${3RDPARTY_DIR}/sparkseecpp-5.0.0 CACHE PATH "Location of SPARKSEE root folder" )
    else()
        message( FATAL " Unable to find SPARKSEE root directory" )
    endif()
endif()

add_definitions( -D_FILE_OFFSET_BITS=64 )
set( SPARKSEE_INCLUDE ${SPARKSEE_ROOT_DIR}/includes/ )
set( SPARKSEE_INCLUDE_DIR ${SPARKSEE_INCLUDE} )
# SPARKSEE includes are not prefixed by sparksee/, we need to include /sparksee ourself ...
include_directories( ${SPARKSEE_INCLUDE_DIR} ${SPARKSEE_INCLUDE_DIR}/sparksee )

if( APPLE )
    set( SPARKSEE_PLATEFORM macos64 )
elseif( WIN32 )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( SPARKSEE_PLATEFORM windows64_VS2012 )
  else()
    set( SPARKSEE_PLATEFORM windows32_VS2012 )
  endif()
elseif( UNIX )
    set( SPARKSEE_PLATEFORM linux64 )
else()
    message( FATAL "SPARKSEE doesn't support this architecture" )
endif()
set( SPARKSEE_LIB_DIR ${SPARKSEE_ROOT_DIR}/lib/${SPARKSEE_PLATEFORM} )
# message( STATUS "SPARKSEE LIB DIR " ${SPARKSEE_LIB_DIR} )
link_directories( ${SPARKSEE_LIB_DIR} )
set( SPARKSEE_LIBRARIES -lsparksee )

# Grab libraries to copy them for export
set( SPARKSEE_LIB_FILES )
file( GLOB SPARKSEE_LIB_FILES ${SPARKSEE_LIB_DIR}/*.* )
# Copy SPARKSEE libs to /lib because install name id is @loader_path
file( COPY ${SPARKSEE_LIB_FILES} DESTINATION ${LIBRARY_OUTPUT_PATH} )

## JSON_SPIRIT
if( USE_JSON )
    message( STATUS "Use json_spirit" )
    add_definitions( -DMLD_USE_JSON )
    if( JSON_SPIRIT_ROOT )  # if overriden by user
        set( JSON_SPIRIT_INCLUDE_DIR ${JSON_SPIRIT_ROOT}/include )
        set( JSON_SPIRIT_LIB_DIR ${JSON_SPIRIT_ROOT}/lib )
        link_directories( ${JSON_SPIRIT_LIB_DIR} )
    endif()
    set( JSON_SPIRIT_LIBRARIES -ljson_spirit )
endif()

#
# PROJECT SETTINGS
#
message( STATUS "Using ${CMAKE_CXX_COMPILER_ID}" )
if( NOT WIN32 )
    add_definitions( -Wall -Wextra -Wno-unused-parameter -Wdeprecated-declarations )
endif()
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

if( SAFE_CHECKS )
    add_definitions( -DMLD_SAFE )
endif()

#### SRC && CUSTOM DEFINITIONS
# Config file
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/mld/config.h )
configure_file( ${RESOURCES_DIR}/mysparksee.cfg.in ${RESOURCES_DIR}/mysparksee.cfg )

# SOURCES
add_subdirectory( src )

# TOOLS
if( BUILD_TOOLS )
    #### TCLAP
    set( TCLAP_INCLUDE_DIR ${3RDPARTY_DIR}/tclap-1.2.1/include )
    include_directories( ${TCLAP_INCLUDE_DIR} )
    add_subdirectory( tools )
endif( BUILD_TOOLS )

# TESTS
if( BUILD_TESTS )
    add_definitions( -DGTEST_USE_OWN_TR1_TUPLE=1 -DGTEST_HAS_TR1_TUPLE=1 -std=c++11 -stdlib=libc++ -Wno-narrowing )
    add_subdirectory( ${3RDPARTY_DIR}/gtest-1.6.0 )
    enable_testing()
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    add_definitions( -DMLD_WITH_TESTS )
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



