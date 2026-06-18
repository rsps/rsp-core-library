cmake_minimum_required(VERSION 3.23...4.33)

include("cmake/parse-version-file.cmake")
parse_version_file(
    FILE "${CMAKE_CURRENT_LIST_DIR}/VERSION"
    PREFIX RSP_CORE_LIB
)

project(rsp-core-lib
    DESCRIPTION "Open-source library for medical device software."
    HOMEPAGE_URL "https://github.com/rsps/rsp-core-library"
    VERSION ${RSP_CORE_LIB_VERSION_MAJOR}.${RSP_CORE_LIB_VERSION_MINOR}.${RSP_CORE_LIB_VERSION_PATCH}
)

option(RSP_CORE_LIB_USE_GFX            "Include graphic components."                      ON)
option(RSP_CORE_LIB_USE_GFX_SDL2       "Use SDL2 for graphic acceleration."               ON)
option(RSP_CORE_LIB_USE_FREETYPE       "Build fonts driver for the FreeType engine."      ON)
option(RSP_CORE_LIB_USE_OPENSSL        "Build with OpenSSL encryption engine."            ON)
option(RSP_CORE_LIB_USE_MBEDTLS        "Build with Mbed TLS encryption engine."           OFF)
option(RSP_CORE_LIB_USE_LIBCURL        "Build with LibCurl network library."              ON)
option(RSP_CORE_LIB_USE_EMBED_HTTP     "Build with Embed HTTP network library."           OFF)
option(RSP_CORE_LIB_USE_WPA_SUPPLICANT "Build with libwpa-client wpa_supplicant library." ON)
option(RSP_CORE_LIB_USE_ZLIB           "Build with ZLib Inflate/Deflate compression."     OFF)
option(RSP_CORE_LIB_USE_STD_THREAD     "Thread architecture to use."                      ON)
option(RSP_CORE_LIB_USE_SYSLOG         "Include syslog interface"                         ON)
option(RSP_CORE_LIB_BUILD_TESTS        "Set to build test binaries"                       ${PROJECT_IS_TOP_LEVEL})

# Append this project's local module paths.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Keep symbol information for exception stack traces
set(CMAKE_ENABLE_EXPORTS TRUE)

# Core library target
add_library(${PROJECT_NAME})

# Dependencies
include("dependencies.cmake")
rsp_core_add_dependencies(${PROJECT_NAME})

# Write version header
include("cmake/generate-version-header.cmake")
add_dependencies(${PROJECT_NAME} generate_version_header)

# Write config header (to let consumers know which features the library was
# built with)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/rsp/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/rsp/config.h"
)

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Compile options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    include("gcc-compile-options")
    target_compile_options(${PROJECT_NAME} PRIVATE "${RSP_GCC_STRICT_COMPILE_OPTIONS}")
endif()

# Macro definitions per build configuration
target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:RSP_CORE_LIB_DEBUG_LOG>")

if(EHTTP_REQUEST_BUFFER_SIZE)
    message(STATUS "Compiling eHttp with request buffer size: ${EHTTP_REQUEST_BUFFER_SIZE}")
    target_compile_definitions(${PROJECT_NAME} PRIVATE EHTTP_REQUEST_BUFFER_SIZE=${EHTTP_REQUEST_BUFFER_SIZE})
endif()

# Library sources
add_subdirectory(src)

# Tests
if(RSP_CORE_LIB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
