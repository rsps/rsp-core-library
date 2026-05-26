# --------------------------------------------------------------------------------------------------------------
# Git utilities
# --------------------------------------------------------------------------------------------------------------

include_guard(GLOBAL)

# Ensure that git is available or this module will not work
find_package(Git REQUIRED)

if(NOT COMMAND "git_get_semver")

    #! git_get_semver : Gets a semver 2.0.0 version string from git describe
    #
    # Combines version tag, SHA1, and dirty state into a single semver string
    # with build metadata: <version>[+<N>.<sha>[.dirty]]
    #
    # @example
    #       git_get_semver(OUTPUT version WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    #       message("${version}")    # e.g. "1.2.3-rc1+0.5e6a7de766aeb2a75eaad246e18229afaa7d5046.dirty"
    #
    # @param OUTPUT <variable>              The output variable for the full semver string
    # @param WORKING_DIRECTORY <string>     Directory from where the git command must be executed
    #
    # @return
    #     [OUTPUT]                          Semver string, e.g. "1.2.3-rc1+0.5e6a7de766aeb2a75eaad246e18229afaa7d5046.dirty"
    #
    function(git_get_semver)
        set(options "")
        set(oneValueArgs OUTPUT WORKING_DIRECTORY)
        set(multiValueArgs "")

        cmake_parse_arguments(INPUT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        # Single git call to get tag, distance, SHA, and dirty state
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --long --dirty --abbrev=40 --match "*[0-9].*[0-9].*[0-9]*"
            WORKING_DIRECTORY "${INPUT_WORKING_DIRECTORY}"
            RESULT_VARIABLE status
            OUTPUT_VARIABLE result
            ERROR_VARIABLE error
            OUTPUT_STRIP_TRAILING_WHITESPACE
            TIMEOUT 3
        )

        if(status EQUAL 0)
            # Parse: <tag>-<N>-g<sha>[-dirty] Regex: the tag can contain hyphens (e.g. "v1.2.3-rc1"), so we anchor from the right
            string(REGEX MATCH "^(.*)-([0-9]+)-g([0-9a-f]+)(-dirty)?$" _ "${result}")
            set(tag "${CMAKE_MATCH_1}")
            set(num_commits "${CMAKE_MATCH_2}")
            set(sha "${CMAKE_MATCH_3}")
            set(dirty "${CMAKE_MATCH_4}")

            # Strip 'v' prefix from tag
            string(REGEX REPLACE "^[vV]" "" version "${tag}")
        else()
            # No matching tag found - fall back to 0.0.0 with SHA and dirty
            message(VERBOSE "${CMAKE_CURRENT_FUNCTION}():\n"
                            "No matching version tag found, using default: 0.0.0\n"
                            "Git error: ${error}")
            set(version "0.0.0")
            set(num_commits "0")

            # Get SHA separately
            execute_process(
                COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                WORKING_DIRECTORY "${INPUT_WORKING_DIRECTORY}"
                OUTPUT_VARIABLE sha
                OUTPUT_STRIP_TRAILING_WHITESPACE
                TIMEOUT 3
            )

            # Get dirty state separately
            execute_process(
                COMMAND ${GIT_EXECUTABLE} diff --quiet
                WORKING_DIRECTORY "${INPUT_WORKING_DIRECTORY}"
                RESULT_VARIABLE dirty_status
                TIMEOUT 3
            )
            if(dirty_status EQUAL 1)
                set(dirty "-dirty")
            else()
                set(dirty "")
            endif()
        endif()

        # Build semver with metadata: <version>+<sha>[.dirty]
        set(full_version "${version}+${num_commits}.${sha}")
        if(dirty)
            set(full_version "${full_version}.dirty")
        endif()

        set("${INPUT_OUTPUT}" "${full_version}")

        return(PROPAGATE "${INPUT_OUTPUT}")
    endfunction()
endif()
