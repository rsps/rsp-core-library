# -------------------------------------------------------------------------------------------------------------- #
# RSP Core Library Tests
# -------------------------------------------------------------------------------------------------------------- #

include("rsp/logging")

# Enable testing
enable_testing()

# -------------------------------------------------------------------------------------------------------------- #
# Test Configuration(s), Assets and other resources...
# -------------------------------------------------------------------------------------------------------------- #
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)

FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG        v2.5.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(doctest)
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")

# Include external project lighttpd from github.
# Uses QUIET FetchContent_Populate mode to avoid deprecation warning.
# This is build using custom target for use during local unit tests
FetchContent_Populate(lighttpd_src
    QUIET
    GIT_REPOSITORY "https://github.com/lighttpd/lighttpd1.4"
    # GIT_REPOSITORY "https://git.lighttpd.net/lighttpd/lighttpd1.4.git" # Original git repo
    GIT_TAG "lighttpd-1.4.82"

    SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/lighttpd_src-src"
    BINARY_DIR "${FETCHCONTENT_BASE_DIR}/lighttpd_src-build"
    SUBBUILD_DIR "${FETCHCONTENT_BASE_DIR}/lighttpd_src-subbuild"
)

# Include & link...
add_custom_target(lighttpd
    COMMENT "Building lighttpd..."

    BYPRODUCTS "${lighttpd_src_BINARY_DIR}/build/lighttpd"
    COMMAND cmake -G${CMAKE_GENERATOR} -DWITH_OPENSSL=ON ${lighttpd_src_SOURCE_DIR} && cmake --build .
    WORKING_DIRECTORY "${lighttpd_src_BINARY_DIR}"
    SOURCES "${lighttpd_src_SOURCE_DIR}/src/lemon.c"
    VERBATIM
)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/webserver/lighttpd.conf.in"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/lighttpd.conf"
)

# Images
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/helpers/testImages"
    DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
)

# Fonts
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/helpers/fonts"
    DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
)

# Helper scripts
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/helpers/scripts"
    DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
)

# Webserver
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/webserver/public"
    DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/"
)
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/webserver/cgi"
    DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/"
)
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/webserver/users.plain"
    DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/"
)
if (NOT EXISTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/ssl/ca")
    message("${COLOR_GREEN}${TEXT_BOLD}Generating TLS certificates${TEXT_BOLD_RESTORE}${COLOR_DEFAULT}")
    execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/webserver/generate-certificates.sh" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver")
endif()

# -------------------------------------------------------------------------------------------------------------- #
# Targets & Compile Options
# -------------------------------------------------------------------------------------------------------------- #
set(TEST_BINARY "rsp-core-lib-test")

add_executable(${TEST_BINARY})
add_dependencies(${TEST_BINARY} lighttpd)

target_include_directories(${TEST_BINARY}
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/helpers"
)

if (NOT RELEASE_BUILD)
    target_compile_options(${TEST_BINARY}
        PRIVATE
            -DGFX_FPS=20
            -DDEBUG_LOG
            -g
            -O0
    )
endif()

target_link_libraries (${TEST_BINARY}
    PUBLIC
        rsp-core-lib
        Threads::Threads
        doctest::doctest
)

# Test files...
file(GLOB_RECURSE SRC_FILES *.cpp)
target_sources(${TEST_BINARY}
    PRIVATE
        ${SRC_FILES}
)

if(NOT CMAKE_CROSSCOMPILING)
    doctest_discover_tests(${TEST_BINARY} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

# -------------------------------------------------------------------------------------------------------------- #
# Install / Export
# -------------------------------------------------------------------------------------------------------------- #

install(
    TARGETS ${TEST_BINARY}
    DESTINATION tgm/
)
