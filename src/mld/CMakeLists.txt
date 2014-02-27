# TOP SRCS

# SOURCES FILES
# Each subdirectory add its sources file to this variable
set( PROJECT_SRCS
    CACHE STRING "Sources Files"
    FORCE
)

add_subdirectory(dao)
add_subdirectory(model)
add_subdirectory(operator)
add_subdirectory(utils)
add_subdirectory(io)

set( CUR_SRCS
    common.h
    config.h
    SparkseeManager.cpp
    GraphTypes.cpp
    MLGBuilder.cpp
)

set( PROJECT_SRCS ${PROJECT_SRCS} ${CUR_SRCS} CACHE STRING "Sources Files" FORCE )

####### PUBLIC HEADERS ########
set( CUR_PUB_HDRS
    common.h
    config.h
    SparkseeManager.h
    Session.h
    MLGBuilder.h
)
file( COPY ${CUR_PUB_HDRS} DESTINATION ${PUBLIC_HDRS_DIR} )


####### TARGET ########
# All sources, add sources group here
set( ALL_SRCS
    ${PROJECT_SRCS} # defined in mld subdirectory
#    ${MOC_HDRS}
)

# Set here libraries to link to the target
set( LIBS_TO_LINK
    ${SPARKSEE_LIBRARIES}
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

## INSTALL LIBS AND EXECUTABLES
install( TARGETS ${PROJECT_NAME}
    FRAMEWORK DESTINATION ${FRAMEWORK_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)
# Copy SPARKSEE_LIBRARIES
install( FILES ${SPARKSEE_LIB_FILES} DESTINATION ${LIB_INSTALL_DIR} )

# Add Install Targets for public headers
if( EXISTS ${PUBLIC_HDRS_DIR} AND IS_DIRECTORY ${PUBLIC_HDRS_DIR} )
    install( DIRECTORY ${PUBLIC_HDRS_DIR} DESTINATION ${INCLUDE_INSTALL_DIR} )
endif()


