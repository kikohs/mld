# Tools can include the public header directory
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

macro(append_tool tool_name tool_srcs)
    set(TOOLS_SRCS ${TOOLS_SRCS} ${tool_srcs} )
    set(TOOLS_NAME ${TOOLS_NAME} ${tool_name})
endmacro(append_tool)

# Append tool here
append_tool(HelloDex hello_dex.cpp)

list(LENGTH TOOLS_SRCS len1)
math(EXPR len2 "${len1} - 1")

foreach(val RANGE ${len2})
    list(GET TOOLS_SRCS ${val} tool_src)
    list(GET TOOLS_NAME ${val} tool_name)

    add_executable( ${tool_name} ${tool_src} )
    target_link_libraries( ${tool_name}
        ${PROJECT_NAME}
        ${LIBCPLUSPLUS}
    )

    get_property( CONFIG_LOCATION TARGET ${tool_name} PROPERTY LOCATION )

    # POST BUILD relinking for Dex library. Otherwise you need to copy libdex to the /bin directory
    if( APPLE )
        add_custom_command(TARGET ${tool_name}
            POST_BUILD
            COMMAND /usr/bin/install_name_tool -change @loader_path/libdex.dylib ${LIBRARY_OUTPUT_PATH}/libdex.dylib ${CONFIG_LOCATION}
            WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
            DEPENDS ${tool_name}
        )
    endif()
endforeach()

