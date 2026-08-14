# -------------------------------------------------------------------------------------------------------------- #
# RSP Core Library Tests
# -------------------------------------------------------------------------------------------------------------- #

enable_testing()

# -------------------------------------------------------------------------------------------------------------- #
# Test Configuration(s), Assets and other resources...
# -------------------------------------------------------------------------------------------------------------- #
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

include("dependencies.cmake")
rsp_core_test_add_dependencies(${TEST_BINARY})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    include("gcc-compile-options")
    target_compile_options(${TEST_BINARY} PRIVATE "${RSP_GCC_STRICT_COMPILE_OPTIONS}")
endif()

target_include_directories(${TEST_BINARY}
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/helpers"
)
set_target_properties(${TEST_BINARY}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Set -DGFX_FPS=20 and -DDEBUG_LOG if debug build
target_compile_definitions(${TEST_BINARY} PRIVATE "$<$<CONFIG:DEBUG>:GFX_FPS=20;DEBUG_LOG>")

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
