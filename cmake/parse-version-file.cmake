include_guard(GLOBAL)

if(NOT COMMAND "parse_version_file")

    #! parse_version_file : Parse a SemVer 2.0.0 version string from a file
    #
    # Reads a file containing a single SemVer 2.0.0 version string and
    # decomposes it into its components. Surrounding whitespace and an optional
    # leading 'v' or 'V' prefix are tolerated.
    #
    # Format: <major>.<minor>.<patch>[-<pre-release>][+<build>]
    #
    # @see https://semver.org/spec/v2.0.0.html
    #
    # @example
    #       parse_version_file(
    #           FILE "${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
    #           PREFIX MYLIB
    #       )
    #       message("${MYLIB_VERSION_MAJOR}.${MYLIB_VERSION_MINOR}.${MYLIB_VERSION_PATCH}")
    #
    # @param FILE <path>      Path to file containing the version string
    # @param PREFIX <name>    Prefix for output variables (e.g. RSP_CORE_LIB)
    #
    # @return
    #     <PREFIX>_VERSION_MAJOR       Major version number
    #     <PREFIX>_VERSION_MINOR       Minor version number
    #     <PREFIX>_VERSION_PATCH       Patch version number
    #     <PREFIX>_VERSION_PRERELEASE  Pre-release identifier (may be empty)
    #     <PREFIX>_VERSION_BUILD       Build metadata (may be empty)
    #
    function(parse_version_file)
        set(options "")
        set(oneValueArgs FILE PREFIX)
        set(multiValueArgs "")

        cmake_parse_arguments(INPUT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

        if(NOT INPUT_FILE)
            message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: FILE argument is required")
        endif()

        if(NOT INPUT_PREFIX)
            message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: PREFIX argument is required")
        endif()

        if(NOT EXISTS "${INPUT_FILE}")
            message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION}: file does not exist: ${INPUT_FILE}")
        endif()

        file(READ "${INPUT_FILE}" version_str)
        string(STRIP "${version_str}" version_str)
        string(REGEX REPLACE "^[vV]" "" version_str "${version_str}")

        # SemVer 2.0.0: <major>.<minor>.<patch>[-<pre-release>][+<build>]
        # Numeric identifiers in major/minor/patch may not have leading zeros.
        # Pre-release and build identifiers are dot-separated alphanumeric+hyphen
        # tokens.
        set(_num "(0|[1-9][0-9]*)")
        set(_ident "[0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*")
        set(_semver_re "^${_num}\\.${_num}\\.${_num}(-(${_ident}))?(\\+(${_ident}))?$")

        if(NOT version_str MATCHES "${_semver_re}")
            message(FATAL_ERROR
                "${CMAKE_CURRENT_FUNCTION}: '${version_str}' (read from ${INPUT_FILE}) "
                "is not a valid SemVer 2.0.0 version string")
        endif()

        set(major      "${CMAKE_MATCH_1}")
        set(minor      "${CMAKE_MATCH_2}")
        set(patch      "${CMAKE_MATCH_3}")
        set(prerelease "${CMAKE_MATCH_5}")
        set(build      "${CMAKE_MATCH_8}")

        set("${INPUT_PREFIX}_VERSION_MAJOR"      "${major}")
        set("${INPUT_PREFIX}_VERSION_MINOR"      "${minor}")
        set("${INPUT_PREFIX}_VERSION_PATCH"      "${patch}")
        set("${INPUT_PREFIX}_VERSION_PRERELEASE" "${prerelease}")
        set("${INPUT_PREFIX}_VERSION_BUILD"      "${build}")

        return(PROPAGATE
            "${INPUT_PREFIX}_VERSION_MAJOR"
            "${INPUT_PREFIX}_VERSION_MINOR"
            "${INPUT_PREFIX}_VERSION_PATCH"
            "${INPUT_PREFIX}_VERSION_PRERELEASE"
            "${INPUT_PREFIX}_VERSION_BUILD"
        )
    endfunction()
endif()
