# OPERATOR SRCS
set( OPERATOR_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractOperator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TSOperator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TSCache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coarseners.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mergers.h
    ${CMAKE_CURRENT_SOURCE_DIR}/selectors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/filters.h
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${OPERATOR_SRCS} CACHE STRING "Sources Files" FORCE )

# Public headers
set( OPERATOR_PUBLIC_HDRS
    AbstractOperator.h
    TSOperator.h
    TSCache.h
    coarseners.h
    mergers.h
    selectors.h
    filters.h
)

set( OPERATOR_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/operator
)

if( NOT EXISTS ${OPERATOR_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${OPERATOR_PUB_HDRS_DIR} )
endif()
file( COPY ${OPERATOR_PUBLIC_HDRS} DESTINATION ${OPERATOR_PUB_HDRS_DIR} )

add_subdirectory(selector)
add_subdirectory(merger)
add_subdirectory(coarsener)
add_subdirectory(filter)
