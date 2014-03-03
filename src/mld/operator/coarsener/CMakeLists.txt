# COARSENER SRCS
set( COARSENER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractCoarsener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NeighborCoarsener.cpp
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${COARSENER_SRCS} CACHE STRING "Sources Files" FORCE )

# Public headers
set( COARSENER_PUBLIC_HDRS
    AbstractCoarsener.h
    NeighborCoarsener.h
)

set( COARSENER_PUB_HDRS_DIR
    ${OPERATOR_PUB_HDRS_DIR}/coarsener
)

if( NOT EXISTS ${COARSENER_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${COARSENER_PUB_HDRS_DIR} )
endif()
file( COPY ${COARSENER_PUBLIC_HDRS} DESTINATION ${COARSENER_PUB_HDRS_DIR} )

