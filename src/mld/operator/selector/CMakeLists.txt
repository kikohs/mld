# SELECTOR SRCS

set( SELECTOR_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSelector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NeighborSelector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HeavyHLinkSelector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/XSelector.cpp
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${SELECTOR_SRCS} CACHE STRING "Sources Files" FORCE )

# Public headers
set( SELECTOR_PUBLIC_HDRS
    AbstractSelector.h
    NeighborSelector.h
    HeavyHLinkSelector.h
    XSelector.h
)

set( SELECTOR_PUB_HDRS_DIR
    ${OPERATOR_PUB_HDRS_DIR}/selector
)

if( NOT EXISTS ${SELECTOR_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${SELECTOR_PUB_HDRS_DIR} )
endif()
file( COPY ${SELECTOR_PUBLIC_HDRS} DESTINATION ${SELECTOR_PUB_HDRS_DIR} )
