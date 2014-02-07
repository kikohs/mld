
set( UTILS_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mutable_priority_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopedTimer.h
    PARENT_SCOPE
)

# Public headers
set( UTILS_PUBLIC_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mutable_priority_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopedTimer.h
    PARENT_SCOPE
)

set( UTILS_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/utils
    PARENT_SCOPE
)
