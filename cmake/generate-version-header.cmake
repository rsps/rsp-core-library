cmake_minimum_required(VERSION 3.25...4.33)

# This CMake script serves dual purposes:
# 1. When included as a module, it defines a custom target to generate a version
#    header file.
# 2. When executed as a script (with -P), it generates the version header file
#    immediately, using the provided variables.
#
# The version header file is generated from a template (version.h.in) and
# includes the RSP_CORE_VERSION, as well as optional git metadata if
# building from a Git repository.

if(CMAKE_SCRIPT_MODE_FILE)
    # Build-time: re-query git and write the header (in case the source tree has
    # changed since last configure)

    # Append Git SHA and dirty to RSP_CORE_LIB_VERSION_BUILD metadata if
    # building from a git repo
    find_package(Git)
    if(GIT_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse --short=12 HEAD
            OUTPUT_VARIABLE _sha
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _sha_result
            ERROR_QUIET
        )

        if(_sha_result EQUAL 0)
            string(APPEND RSP_CORE_LIB_VERSION_BUILD "git.${_sha}")

            execute_process(
                COMMAND "${GIT_EXECUTABLE}" diff-index --quiet HEAD --
                RESULT_VARIABLE _dirty_result
                ERROR_QUIET
            )

            if(_dirty_result EQUAL 1)
                string(APPEND RSP_CORE_LIB_VERSION_BUILD ".dirty")
            endif()
        endif()
    endif()

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
            -DRSP_CORE_LIB_VERSION_MAJOR="${RSP_CORE_LIB_VERSION_MAJOR}"
            -DRSP_CORE_LIB_VERSION_MINOR="${RSP_CORE_LIB_VERSION_MINOR}"
            -DRSP_CORE_LIB_VERSION_PATCH="${RSP_CORE_LIB_VERSION_PATCH}"
            -DRSP_CORE_LIB_VERSION_PRERELEASE="${RSP_CORE_LIB_VERSION_PRERELEASE}"
            -DRSP_CORE_LIB_VERSION_BUILD="${RSP_CORE_LIB_VERSION_BUILD}"
            -DVERSION_H_IN="${_version_h_in}"
            -DVERSION_H_OUT="${_version_h_tmp}"
            -P "${CMAKE_CURRENT_LIST_FILE}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_version_h_tmp}" "${_version_h_out}"
        COMMAND ${CMAKE_COMMAND} -E rm -f "${_version_h_tmp}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Writing version.h"
    )
endif()
