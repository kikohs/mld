## Since we are testing we have access to all the headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mld
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/
)

enable_testing(true)

macro(append_test test_name test_srcs)
    set(TESTS_SRCS ${TESTS_SRCS} ${test_srcs} )
    set(TESTS_NAME ${TESTS_NAME} ${test_name})
endmacro(append_test)


# APPEND TESTS HERE
# DAO
append_test(SNodeDaoTest dao/SNodeDaoTest.cpp)
append_test(LayerDaoTest dao/LayerDaoTest.cpp)
append_test(LinkDaoTest dao/LinkDaoTest.cpp)
append_test(MLGDaoTest dao/MLGDaoTest.cpp)

# OPERATOR
append_test(MergerTest operator/MergerTest.cpp)
append_test(CoarsenerTest operator/CoarsenerTest.cpp)
append_test(XSelectorTest operator/XSelectorTest.cpp)

# TOP level test
append_test(MLGBuilderTest MLGBuilderTest.cpp)
append_test(TimerTest TimerTest.cpp)

#### Create executables
list(LENGTH TESTS_SRCS len1)
math(EXPR len2 "${len1} - 1")

foreach( val RANGE ${len2} )
list(GET TESTS_SRCS ${val} test_src)
list(GET TESTS_NAME ${val} test_name)
add_executable( ${test_name} ${test_src} )
target_link_libraries( ${test_name}
    gtest
    gtest_main
    ${PROJECT_NAME}
    ${LIBCPLUSPLUS}
)
get_property( CONFIG_LOCATION TARGET ${test_name} PROPERTY LOCATION )
add_test( NAME ${test_name} COMMAND ${test_name} )


# POST BUILD relinking for Sparksee library. Otherwise you need to copy libsparksee to the /bin directory
if( APPLE )
    add_custom_command(TARGET ${test_name}
        POST_BUILD
        COMMAND /usr/bin/install_name_tool -change @loader_path/libsparksee.dylib ${LIBRARY_OUTPUT_PATH}/libsparksee.dylib ${CONFIG_LOCATION}
        WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
        DEPENDS ${test_name}
    )
endif()

endforeach()
