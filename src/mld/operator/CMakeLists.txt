# OPERATOR SRCS
set( OPERATOR_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractOperator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractCoarsener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSelector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractMerger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/coarseners.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mergers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/selectors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/XCoarsener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/XSelector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MultiAdditiveMerger.cpp
    PARENT_SCOPE
)

# Public headers
set( OPERATOR_PUBLIC_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractOperator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractCoarsener.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractSelector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractMerger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/coarseners.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mergers.h
    ${CMAKE_CURRENT_SOURCE_DIR}/selectors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/XCoarsener.h
    ${CMAKE_CURRENT_SOURCE_DIR}/XSelector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MultiAdditiveMerger.h
    PARENT_SCOPE
)

set( OPERATOR_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/operator
    PARENT_SCOPE
)
