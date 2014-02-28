# MERGER SRCS

set( MERGER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractMerger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AdditiveNeighborMerger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mergers.cpp
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${MERGER_SRCS} CACHE STRING "Sources Files" FORCE )

# Public headers
set( MERGER_PUBLIC_HDRS
    AbstractMerger.h
    AdditiveNeighborMerger.h
)

set( MERGER_PUB_HDRS_DIR
    ${OPERATOR_PUB_HDRS_DIR}/merger
)

if( NOT EXISTS ${MERGER_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${MERGER_PUB_HDRS_DIR} )
endif()
file( COPY ${MERGER_PUBLIC_HDRS} DESTINATION ${MERGER_PUB_HDRS_DIR} )

