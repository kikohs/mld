
set( MLD ${CMAKE_CURRENT_SOURCE_DIR} )
set( DAO ${MLD}/dao )
set( MODEL ${MLD}/model )
set( OPERATOR ${MLD}/operator )

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

# OPERATOR SRCS
set( OPERATOR_SRCS
    ${OPERATOR}/AbstractOperator.cpp
)

# SOURCES FILES
set( project_SRCS
    mld/common.h
    mld/config.h
    mld/DexManager.cpp
    mld/Graph_types.cpp
    mld/MLGBuilder.cpp
    ${DAO_SRCS}
    ${MODEL_SRCS}
    ${OPERATOR_SRCS}

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
    ${MLD}/MLGBuilder.h

    PARENT_SCOPE
)

# DAO
set( DAO_PUBLIC_HDRS
    ${DAO}/MLGDao.h
    ${DAO}/AbstractDao.h
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

# OPERATOR
set( OPERATOR_PUBLIC_HDRS
    ${OPERATOR}/AbstractOperator.h
    PARENT_SCOPE
)

set( OPERATOR_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/operator
    PARENT_SCOPE
)

