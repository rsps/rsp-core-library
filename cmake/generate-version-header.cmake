cmake_minimum_required(VERSION 3.25)

include("${CMAKE_CURRENT_LIST_DIR}/git.cmake")

# Dual mode: serves as both the include module and -P script
if(CMAKE_SCRIPT_MODE_FILE)
    # Build-time: re-query git and write the header (in case the source tree has
    # changed since last configure)
    git_get_semver(OUTPUT RSP_CORE_LIB_SEMVER WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
    configure_file("${VERSION_H_IN}" "${VERSION_H_OUT}" @ONLY)
else()
    # Configure-time: define the build target
    set(_version_h_in "${CMAKE_CURRENT_SOURCE_DIR}/include/rsp/version.h.in")
    set(_version_h_tmp "${CMAKE_CURRENT_BINARY_DIR}/include/rsp/version.h.tmp")
    set(_version_h_out "${CMAKE_CURRENT_BINARY_DIR}/include/rsp/version.h")

    # Add a custom target to generate version header. Use an intermediate temp
    # file to avoid unnecessary rebuilds when the version hasn't changed.
    add_custom_target(generate_version_header
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/include"
        COMMAND ${CMAKE_COMMAND}
            -DPROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
            -DVERSION_H_IN="${_version_h_in}"
            -DVERSION_H_OUT="${_version_h_tmp}"
            -P "${CMAKE_CURRENT_LIST_FILE}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_version_h_tmp}" "${_version_h_out}"
        COMMAND ${CMAKE_COMMAND} -E rm -f "${_version_h_tmp}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Writing version.h"
    )
endif()
