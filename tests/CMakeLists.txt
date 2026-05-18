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

include("external/doctest")
include("external/lighttpd")

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
