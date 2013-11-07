
set( MLD ${CMAKE_CURRENT_SOURCE_DIR} )
set( DAO ${MLD}/dao )
set( MODEL ${MLD}/model )

# DAO SRCS
set( DAO_SRCS
    ${DAO}/SNodeDao.cpp
    ${DAO}/LayerDao.cpp
    ${DAO}/LinkDao.cpp
    ${DAO}/MLGDao.cpp
    ${DAO}/AbstractDao.cpp
)

# MODEL SRCS
set( MODEL_SRCS
    ${MODEL}/Layer.cpp
    ${MODEL}/SuperNode.cpp
    ${MODEL}/Link.cpp
)

# SOURCES FILES
set( project_SRCS
    mld/common.h
    mld/config.h
    mld/DexManager.cpp
    mld/Graph_types.cpp
    ${DAO_SRCS}
    ${MODEL_SRCS}

    PARENT_SCOPE
)

####### PUBLIC HEADERS ########

# Current dir
set( CUR_PUB_HDRS
    ${MLD}/common.h
    ${MLD}/config.h
    ${MLD}/log.h
    ${MLD}/DexManager.h
    ${MLD}/Session.h

    PARENT_SCOPE
)

# DAO
set( DAO_PUBLIC_HDRS
    ${DAO}/SNodeDao.h
    ${DAO}/LayerDao.h
    ${DAO}/MLGDao.h
    ${DAO}/AbstractDao.h
    ${DAO}/LinkDao.h
    PARENT_SCOPE
)

set( DAO_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/dao
    PARENT_SCOPE
)

# MODEL
set( MODEL_PUBLIC_HDRS
    ${MODEL}/Layer.h
    ${MODEL}/SuperNode.h
    ${MODEL}/Link.h

    PARENT_SCOPE
)

set( MODEL_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/model
    PARENT_SCOPE
)


