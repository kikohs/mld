set( MLD ${CMAKE_CURRENT_SOURCE_DIR} )

# SOURCES FILES
set( project_SRCS
    mld/common.h
    mld/config.h
    mld/DexManager.cpp
    PARENT_SCOPE
)

# PUBLIC HEADERS
set( PUBLIC_HDRS
    ${MLD}/common.h
    ${MLD}/config.h
    ${MLD}/DexManager.h
    ${MLD}/Session.h
    PARENT_SCOPE
)


