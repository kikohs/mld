# UTILS SRCS
set( UTILS_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mutable_priority_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopedTimer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ProgressDisplay.h
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${UTILS_SRCS} CACHE STRING "Sources Files" FORCE )

# Public headers
set( UTILS_PUBLIC_HDRS
    log.h
    mutable_priority_queue.h
    Timer.h
    ScopedTimer.h
    ProgressDisplay.h
)

set( UTILS_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/utils
)

if( NOT EXISTS ${UTILS_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${UTILS_PUB_HDRS_DIR} )
endif()
file( COPY ${UTILS_PUBLIC_HDRS} DESTINATION ${UTILS_PUB_HDRS_DIR} )
