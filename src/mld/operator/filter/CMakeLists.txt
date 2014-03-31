# FILTER SRCS
set( FILTER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/FilterFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractTimeVertexFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TimeVertexMeanFilter.cpp
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${FILTER_SRCS} CACHE STRING "Sources Files" FORCE )

# Public headers
set( FILTER_PUBLIC_HDRS
    FilterFactory.h
    AbstractTimeVertexFilter.h
    TimeVertexMeanFilter.h
)

set( FILTER_PUB_HDRS_DIR
    ${OPERATOR_PUB_HDRS_DIR}/filter
)

if( NOT EXISTS ${FILTER_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${FILTER_PUB_HDRS_DIR} )
endif()
file( COPY ${FILTER_PUBLIC_HDRS} DESTINATION ${FILTER_PUB_HDRS_DIR} )

