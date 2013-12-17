# Copy macro
include(Copy)

add_subdirectory(dao)

add_subdirectory(model)

add_subdirectory(operator)

add_subdirectory(utils)

# SOURCES FILES
set( project_SRCS
    common.h
    config.h
    DexManager.cpp
    Graph_types.cpp
    MLGBuilder.cpp
    ${DAO_SRCS}
    ${MODEL_SRCS}
    ${OPERATOR_SRCS}
    ${UTILS_SRCS}
)

####### PUBLIC HEADERS ########

# Current dir
set( CUR_PUB_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/DexManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Session.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MLGBuilder.h
)

# All sources, add sources group here
set( ALL_SRCS
    ${project_SRCS} # defined in mld subdirectory
#    ${MOC_HDRS}
)

# Set here libraries to link to the target
set( LIBS_TO_LINK
    ${DEX_LIBRARIES}
    ${LIBCPLUSPLUS}
#    ${Boost_LIBRARIES}
)

# Shared library
add_library( ${PROJECT_NAME}
    SHARED
    ${ALL_SRCS}
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_SOVERSION}
    INSTALL_NAME_DIR ${LIB_INSTALL_DIR}
    DEFINE_SYMBOL MLD_API_EXPORTS
    OUTPUT_NAME ${PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1
)

target_link_libraries( ${PROJECT_NAME}
    ${LIBS_TO_LINK}
)


# Copy public headers in directory
# You need to put the list (PUBLIC_HDRS) with quotes, otherwise
# the macro only takes the first element.
# NICE !
# VARIABLE PUBLIC_HDRS definied in subdirectory
COPY_FILES_INTO_DIRECTORY_IF_CHANGED( "${CUR_PUB_HDRS}" ${PUBLIC_HDRS_DIR} ${PROJECT_NAME} )
# DAO
if( NOT EXISTS ${DAO_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${DAO_PUB_HDRS_DIR} )
endif()
COPY_FILES_INTO_DIRECTORY_IF_CHANGED( "${DAO_PUBLIC_HDRS}" ${DAO_PUB_HDRS_DIR} ${PROJECT_NAME} )
# MODEL
if( NOT EXISTS ${MODEL_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${MODEL_PUB_HDRS_DIR} )
endif()
COPY_FILES_INTO_DIRECTORY_IF_CHANGED( "${MODEL_PUBLIC_HDRS}" ${MODEL_PUB_HDRS_DIR} ${PROJECT_NAME} )
# OPERATOR
if( NOT EXISTS ${OPERATOR_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${OPERATOR_PUB_HDRS_DIR} )
endif()
COPY_FILES_INTO_DIRECTORY_IF_CHANGED( "${OPERATOR_PUBLIC_HDRS}" ${OPERATOR_PUB_HDRS_DIR} ${PROJECT_NAME} )
# UTILS
if( NOT EXISTS ${UTILS_PUB_HDRS_DIR} )
    file( MAKE_DIRECTORY ${UTILS_PUB_HDRS_DIR} )
endif()
COPY_FILES_INTO_DIRECTORY_IF_CHANGED( "${UTILS_PUB_HDRS_DIR}" ${UTILS_PUB_HDRS_DIR} ${PROJECT_NAME} )


## INSTALL LIBS AND EXECUTABLES
install( TARGETS ${PROJECT_NAME}
    FRAMEWORK DESTINATION ${FRAMEWORK_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)
# Copy DEX_LIBRARIES
install( FILES ${DEX_LIB_FILES} DESTINATION ${LIB_INSTALL_DIR} )



