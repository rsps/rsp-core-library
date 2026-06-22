# Findlighttpd.cmake
# ------------------
# Find the system-installed lighttpd executable and its modules directory.
#
# Result Variables:
#
#   lighttpd_FOUND        - System has lighttpd
#   lighttpd_EXECUTABLE   - Full path to the lighttpd executable
#   lighttpd_MODULES_DIR  - Directory containing lighttpd module .so files
#   lighttpd_VERSION      - Full version string (e.g. "1.4.74")
#   lighttpd_VERSION_MAJOR
#   lighttpd_VERSION_MINOR
#   lighttpd_VERSION_PATCH
#
# Imported Targets:
#
#   lighttpd::lighttpd - Imported executable target
#

find_program(lighttpd_EXECUTABLE
    NAMES lighttpd
    HINTS /usr/sbin /usr/bin /usr/local/sbin /usr/local/bin
)

# Extract version from "lighttpd/X.Y.Z ..." printed by lighttpd -v
if(lighttpd_EXECUTABLE AND NOT lighttpd_VERSION)
    execute_process(
        COMMAND "${lighttpd_EXECUTABLE}" -v
        OUTPUT_VARIABLE _lighttpd_version_out
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(_lighttpd_version_out MATCHES "lighttpd/([0-9]+)\\.([0-9]+)\\.([0-9]+)")
        set(lighttpd_VERSION       "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
        set(lighttpd_VERSION_MAJOR "${CMAKE_MATCH_1}")
        set(lighttpd_VERSION_MINOR "${CMAKE_MATCH_2}")
        set(lighttpd_VERSION_PATCH "${CMAKE_MATCH_3}")
    endif()
    unset(_lighttpd_version_out)
endif()

# Derive the modules directory relative to the install prefix of the executable.
if(lighttpd_EXECUTABLE)
    get_filename_component(_lighttpd_bin_dir "${lighttpd_EXECUTABLE}" DIRECTORY)
    get_filename_component(_lighttpd_prefix "${_lighttpd_bin_dir}" DIRECTORY)

    find_path(lighttpd_MODULES_DIR
        NAMES mod_auth.so mod_cgi.so mod_status.so
        HINTS
            "${_lighttpd_prefix}/lib/lighttpd"
            "${_lighttpd_prefix}/lib64/lighttpd"
            /usr/lib/lighttpd
            /usr/lib64/lighttpd
            /usr/local/lib/lighttpd
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(lighttpd
    REQUIRED_VARS lighttpd_EXECUTABLE lighttpd_MODULES_DIR
    VERSION_VAR lighttpd_VERSION
)

if(lighttpd_FOUND)
    if(NOT TARGET lighttpd::lighttpd)
        add_executable(lighttpd::lighttpd IMPORTED)
        set_target_properties(lighttpd::lighttpd PROPERTIES
            IMPORTED_LOCATION "${lighttpd_EXECUTABLE}"
        )
    endif()
endif()
