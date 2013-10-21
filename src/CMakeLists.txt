include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Copy macro
include(Copy)

file( GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp )

# All sources, add sources group here
set( ALL_SRCS
    ${project_SRCS}
#    ${MOC_HDRS}
)

# Set here libraries to link to the target
set( LIBS_TO_LINK
    ${DEX_LIBRARIES}
    ${LIBCPLUSPLUS}
#    ${Boost_LIBRARIES}
)

# Public headers
set( SRCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mld )
set( PUBLIC_HDRS
    ${SRCS_DIR}/common.h
    ${SRCS_DIR}/config.h
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
COPY_FILES_INTO_DIRECTORY_IF_CHANGED( "${PUBLIC_HDRS}" ${PUBLIC_HDRS_DIR} ${PROJECT_NAME} )

## INSTALL LIBS AND EXECUTABLES
install( TARGETS ${PROJECT_NAME}
    FRAMEWORK DESTINATION ${FRAMEWORK_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)
# Copy DEX_LIBRARIES
install( FILES ${DEX_LIB_FILES} DESTINATION ${LIB_INSTALL_DIR} )
