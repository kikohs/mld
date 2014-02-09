
set( IO_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphImporter.cpp
    PARENT_SCOPE
)

# Public headers
set( IO_PUBLIC_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphImporter.h
    PARENT_SCOPE
)

set( IO_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/io
    PARENT_SCOPE
)
