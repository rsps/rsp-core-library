# -------------------------------------------------------------------------------------------------------------- #
# RSP Core Library Tests
# -------------------------------------------------------------------------------------------------------------- #

enable_testing()

include(FetchContent)
FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG        v2.5.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(doctest)
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")

# Use ExternalProject to isolate the lighttpd build
include(ExternalProject)
ExternalProject_Add(lighttpd
    GIT_REPOSITORY  https://github.com/lighttpd/lighttpd1.4
    # GIT_REPOSITORY https://git.lighttpd.net/lighttpd/lighttpd1.4.git # Original git repo
    GIT_TAG         lighttpd-1.4.82
    GIT_SHALLOW     TRUE
    TMP_DIR         "${FETCHCONTENT_BASE_DIR}/lighttpd-tmp"
    STAMP_DIR       "${FETCHCONTENT_BASE_DIR}/lighttpd-stamp"
    SOURCE_DIR      "${FETCHCONTENT_BASE_DIR}/lighttpd-src"
    BINARY_DIR      "${FETCHCONTENT_BASE_DIR}/lighttpd-build"
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_ARGS      --log-level=WARNING -Wno-deprecated -DCMAKE_BUILD_TYPE=Release -DWITH_OPENSSL=ON
    INSTALL_COMMAND ""  # skip install step
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/webserver/lighttpd.conf.in"
    "${CMAKE_BINARY_DIR}/webserver/lighttpd.conf"
)

# Images
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/helpers/testImages"
    DESTINATION "${CMAKE_BINARY_DIR}/"
)

# Fonts
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/helpers/fonts"
    DESTINATION "${CMAKE_BINARY_DIR}/"
)

# Helper scripts
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/helpers/scripts"
    DESTINATION "${CMAKE_BINARY_DIR}/"
)

# Webserver
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/webserver/public"
    DESTINATION "${CMAKE_BINARY_DIR}/webserver/"
)
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/webserver/cgi"
    DESTINATION "${CMAKE_BINARY_DIR}/webserver/"
)
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/webserver/users.plain"
    DESTINATION "${CMAKE_BINARY_DIR}/webserver/"
)
if (NOT EXISTS "${CMAKE_BINARY_DIR}/webserver/ssl/ca")
    message("Generating TLS certificates")
    execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/webserver/generate-certificates.sh" "${CMAKE_BINARY_DIR}/webserver")
endif()

# -------------------------------------------------------------------------------------------------------------- #
# Targets & Compile Options
# -------------------------------------------------------------------------------------------------------------- #
set(TEST_BINARY "rsp-core-lib-test")

add_executable(${TEST_BINARY})
add_dependencies(${TEST_BINARY} lighttpd)

set_target_properties(${TEST_BINARY}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_include_directories(${TEST_BINARY}
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/helpers"
)

# Set -DGFX_FPS=20 and -DDEBUG_LOG if debug build
target_compile_definitions(${TEST_BINARY} PRIVATE "$<$<CONFIG:DEBUG>:GFX_FPS=20;DEBUG_LOG>")

target_link_libraries(${TEST_BINARY}
    PUBLIC
        rsp-core-lib
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
