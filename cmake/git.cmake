# -------------------------------------------------------------------------------------------------------------- #
# Git utilities
# -------------------------------------------------------------------------------------------------------------- #

# Ensure that git is available or this module will not work
find_package(Git REQUIRED)

if (NOT COMMAND "git_find_version_tag")

    #! git_find_version_tag : Finds the nearest git tag that matches a version pattern
    #
    # @see https://git-scm.com/docs/git-describe
    #
    # @example
    #       git_find_version_tag(OUTPUT version WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    #       message("${version}") # 3.22.3
    #
    # @param OUTPUT <variable>          The output variable to assign the found version tag
    # @param WORKING_DIRECTORY <string> Directory from where the git command must be executed from
    # @param [MATCH_PATTERN <string>]   Optional glob match pattern for tag. Defaults to "*[0-9].*[0-9].*[0-9]*"
    #                                   when none is specified.
    # @param [DEFAULT <string>]         Optional default version string to return, when no version tag
    #                                   could be found. Defaults to "0.0.0"
    # @param [EXIT_ON_FAILURE]          OPTION: Throws fatal error if unable to find version tag, regardless
    #                                   of the [DEFAULT] argument.
    #
    # @return
    #     [OUTPUT]                      The resulting version tag, e.g. "v1.5.22"
    #
    # @throws If [EXIT_ON_FAILURE] option is specified and unable to find version tag
    #
    function(git_find_version_tag)
        set(options EXIT_ON_FAILURE) # N/A
        set(oneValueArgs OUTPUT WORKING_DIRECTORY MATCH_PATTERN DEFAULT)
        set(multiValueArgs "") # N/A

        cmake_parse_arguments(INPUT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        # requires_arguments("OUTPUT;WORKING_DIRECTORY" INPUT)

        # Resolve optional arguments
        if (NOT DEFINED INPUT_MATCH_PATTERN)
            set(INPUT_MATCH_PATTERN "*[0-9].*[0-9].*[0-9]*")
        endif ()
        if (NOT DEFINED INPUT_DEFAULT)
            set(INPUT_DEFAULT "0.0.0")
        endif ()

        # Run git command
        execute_process(
            # Use "git describe --tags" to obtain the nearest tag, which matches the glob pattern.
            # And alternative could be "git tag --list --sort=-version:refname". Yet, a list of tags
            # would then have to be processed...
            COMMAND ${GIT_EXECUTABLE} describe --tags --match "${INPUT_MATCH_PATTERN}" --abbrev=0
            WORKING_DIRECTORY "${INPUT_WORKING_DIRECTORY}"
            RESULT_VARIABLE status
            OUTPUT_VARIABLE result
            ERROR_VARIABLE error
            OUTPUT_STRIP_TRAILING_WHITESPACE
            TIMEOUT 1
        )

        # Use fallback version if none could be found
        if (NOT status EQUAL 0)
            # Abort if requested to exit on failure
            if (INPUT_EXIT_ON_FAILURE)
                message(FATAL_ERROR
                    "No version tag found\n"
                    "Git exit code: ${status}\n"
                    "Git error message: ${error}"
                )
            endif ()

            # Debug
            message(VERBOSE
                "${CMAKE_CURRENT_FUNCTION}():\n"
                "No version tag found, using default version: ${INPUT_DEFAULT}\n"
                "Git exit code: ${status}\n"
                "Git error message: ${error}"
                " - called from ${CMAKE_CURRENT_LIST_FILE}\n"
            )

            # Use default version...
            set(result "${INPUT_DEFAULT}")
        endif ()

        # Set the resulting version
        set("${INPUT_OUTPUT}" "${result}")

        return(
            PROPAGATE
            "${INPUT_OUTPUT}"
        )
    endfunction()
endif ()
