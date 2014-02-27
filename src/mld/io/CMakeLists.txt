# IO SRCS
set( IO_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphImporter.cpp
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${IO_SRCS} CACHE STRING "Sources Files" FORCE )

# Public headers
set( IO_PUBLIC_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphImporter.h
)

set( IO_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/io
)

if( NOT EXISTS ${IO_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${IO_PUB_HDRS_DIR} )
endif()
file( COPY ${IO_PUBLIC_HDRS} DESTINATION ${IO_PUB_HDRS_DIR} )
