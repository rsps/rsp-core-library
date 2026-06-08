cmake_minimum_required(VERSION 3.23...3.30)

# Get CMake-friendly version from Git tag
include("cmake/git.cmake")
git_get_version(OUTPUT RSP_CORE_LIB_VERSION WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

project(rsp-core-lib
    DESCRIPTION "Open-source library for medical device software."
    HOMEPAGE_URL "https://github.com/rsps/rsp-core-library"
    VERSION ${RSP_CORE_LIB_VERSION}
)

option(GFX "Include graphic components." ON)
option(SDL2 "Use SDL2 for graphic acceleration." ON)
option(FREETYPE_FONTS "Build fonts driver for the FreeType engine." ON)
option(OPENSSL_CRYPTO "Build with OpenSSL encryption engine." ON)
option(MBEDTLS_CRYPTO "Build with Mbed TLS encryption engine." OFF)
option(NET_LIBCURL "Build with LibCurl network library." ON)
option(NET_EMBED_HTTP "Build with Embed HTTP network library." OFF)
option(BUILD_TESTING "Set to build test binaries" ${PROJECT_IS_TOP_LEVEL}) # Enabled by default if this is the top-level project.
option(ZLIB "Build with ZLib Inflate/Deflate compression." OFF)
option(NET_WPA_SUPPLICANT "Build with libwpa-client wpa_supplicant library." ON)
option(STD_THREAD "Thread architecture to use." ON) # C++ std::thread
option(SYSLOG "Include syslog interface" ON)

# Append this project's local module paths.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 23)

# Keep symbol information for exception stack traces
set(CMAKE_ENABLE_EXPORTS TRUE)

# Core library target
add_library(${PROJECT_NAME} STATIC)

# Dependencies
include("dependencies.cmake")
rsp_core_add_dependencies(${PROJECT_NAME})

include("cmake/generate-version-header.cmake")
add_dependencies(${PROJECT_NAME} generate_version_header)

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>;${CMAKE_CURRENT_BINARY_DIR}/include
        $<INSTALL_INTERFACE:include>
)

# Compile options
include("cmake/gcc-compile-options.cmake")
target_compile_options(${PROJECT_NAME} PRIVATE "${RSP_GCC_STRICT_COMPILE_OPTIONS}")

# Macro definitions per build configuration
target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:DEBUG_LOG>")

if(EHTTP_REQUEST_BUFFER_SIZE)
    message(STATUS "Compiling eHttp with request buffer size: ${EHTTP_REQUEST_BUFFER_SIZE}")
    target_compile_definitions(${PROJECT_NAME} PRIVATE EHTTP_REQUEST_BUFFER_SIZE=${EHTTP_REQUEST_BUFFER_SIZE})
endif()

# Library sources
add_subdirectory(src)

# Tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
