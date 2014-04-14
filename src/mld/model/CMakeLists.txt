# MODEL SRCS
set( MODEL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/GraphObject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Layer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Link.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TimeSeries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VirtualGraph.cpp
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${MODEL_SRCS} CACHE STRING "Sources Files" FORCE )

# Public headers
set( MODEL_PUBLIC_HDRS
    GraphObject.h
    Layer.h
    Node.h
    Link.h
    TimeSeries.h
    VirtualGraph.h
)

set( MODEL_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/model
)

# MODEL
if( NOT EXISTS ${MODEL_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${MODEL_PUB_HDRS_DIR} )
endif()
file( COPY ${MODEL_PUBLIC_HDRS} DESTINATION ${MODEL_PUB_HDRS_DIR} )
