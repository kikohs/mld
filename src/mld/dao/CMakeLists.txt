# DAO SRCS
set( DAO_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/NodeDao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LayerDao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LinkDao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/OwnsDao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MLGDao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractDao.cpp
)

# Add to global variable
set( PROJECT_SRCS ${PROJECT_SRCS} ${DAO_SRCS} CACHE STRING "Sources Files" FORCE )


# Public headers
set( DAO_PUBLIC_HDRS
    MLGDao.h
    AbstractDao.h
)

set( DAO_PUB_HDRS_DIR
    ${PUBLIC_HDRS_DIR}/dao
)

# DAO
if( NOT EXISTS ${DAO_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${DAO_PUB_HDRS_DIR} )
endif()
file( COPY ${DAO_PUBLIC_HDRS} DESTINATION ${DAO_PUB_HDRS_DIR} )
