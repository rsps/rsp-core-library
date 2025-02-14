# -------------------------------------------------------------------------------------------------------------- #
# RSP Core Library Tests
# -------------------------------------------------------------------------------------------------------------- #

# Enable testing
enable_testing()

# -------------------------------------------------------------------------------------------------------------- #
# Test Configuration(s), Assets and other resources...
# -------------------------------------------------------------------------------------------------------------- #

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
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/webserver/ssl"
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

# -------------------------------------------------------------------------------------------------------------- #
# Targets & Compile Options
# -------------------------------------------------------------------------------------------------------------- #

add_dependencies(${TEST_BINARY} doctest lighttpd)

target_include_directories(${TEST_BINARY}
    PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_BINARY_DIR}/src"
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
    rsp-core-lib
    Threads::Threads
)

# Add "test binary" as a CTest test...
add_test("tests" ${TEST_BINARY})

# Test files...
file(GLOB_RECURSE SRC_FILES *.cpp)
target_sources(${TEST_BINARY}
    PRIVATE
        ${SRC_FILES}
)

# -------------------------------------------------------------------------------------------------------------- #
# Install / Export
# -------------------------------------------------------------------------------------------------------------- #

install(
    TARGETS ${TEST_BINARY}
    DESTINATION tgm/
)
