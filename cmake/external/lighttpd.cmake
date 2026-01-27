# -------------------------------------------------------------------------------------------------------------- #
# Lighttpd
# @see https://redmine.lighttpd.net/projects/lighttpd
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing Lighttpd")

# -------------------------------------------------------------------------------------------------------------- #

set(LIGHTTPD_VERSION "1.4.82")

# Include external project lighttpd from github.
# Uses QUIET FetchContent_Populate mode to avoid deprecation warning.
# This is build using custom target for use during local unit tests
FetchContent_Populate(lighttpd_src
    QUIET
    GIT_REPOSITORY "https://github.com/lighttpd/lighttpd1.4"
    # GIT_REPOSITORY "https://git.lighttpd.net/lighttpd/lighttpd1.4.git" # Original git repo
    GIT_TAG "lighttpd-${LIGHTTPD_VERSION}"

    SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/lighttpd_src-src"
    BINARY_DIR "${FETCHCONTENT_BASE_DIR}/lighttpd_src-build"
    SUBBUILD_DIR "${FETCHCONTENT_BASE_DIR}/lighttpd_src-subbuild"
)

# Debug
message(STATUS "Using Lighttpd v${LIGHTTPD_VERSION}")

# Include & link...
add_custom_target(lighttpd
    COMMENT "Building lighttpd..."

    BYPRODUCTS "${lighttpd_src_BINARY_DIR}/build/lighttpd"
    COMMAND cmake -G${CMAKE_GENERATOR} -DWITH_OPENSSL=ON ${lighttpd_src_SOURCE_DIR} && cmake --build .
    WORKING_DIRECTORY "${lighttpd_src_BINARY_DIR}"
    SOURCES "${lighttpd_src_SOURCE_DIR}/src/lemon.c"
    VERBATIM
)

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
