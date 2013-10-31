set( MLD ${CMAKE_CURRENT_SOURCE_DIR} )

# SOURCES FILES
set( project_SRCS
    mld/common.h
    mld/config.h
    mld/DexManager.cpp
    mld/Graph_types.cpp
    mld/dao/SNodeDao.cpp
    mld/dao/LayerDao.cpp
    mld/model/SuperNode.cpp
    mld/model/Layer.cpp

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


