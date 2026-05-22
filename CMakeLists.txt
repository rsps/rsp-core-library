cmake_minimum_required(VERSION 3.23...3.30)

# Get project's version from Git
include("cmake/git.cmake")
git_find_version_tag(
    OUTPUT RSP_CORE_LIB_VERSION
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEFAULT "0.0.0"
)

project(rsp-core-lib
    DESCRIPTION "Open-source library for medical device software."
    HOMEPAGE_URL "https://github.com/rsps/rsp-core-library"
    VERSION ${RSP_CORE_LIB_VERSION}
)

message(STATUS "Project version: ${RSP_CORE_LIB_VERSION}")

option(ARCH_ARM "Set to cross-compile for ARM CPU" OFF)
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

# Set toolchain file, if needed.
if(ARCH_ARM)
    # Set CMAKE_<LANG>_FLAGS to prevent toolchain file from overriding these flags when it is included.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64")
    set(CMAKE_TOOLCHAIN_FILE "/opt/arm-buildroot-linux-gnueabihf_sdk-buildroot/share/buildroot/toolchainfile.cmake")
endif()

set(CMAKE_CXX_STANDARD 23)

# Keep symbol information for exception stack traces
set(CMAKE_ENABLE_EXPORTS TRUE)

# Core library target
add_library(${PROJECT_NAME} STATIC)

# Dependencies
include("dependencies.cmake")
rsp_core_add_dependencies(${PROJECT_NAME})

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Compile options
include("cmake/gcc-compile-options.cmake")
target_compile_options(${PROJECT_NAME} PRIVATE "${RSP_GCC_STRICT_COMPILE_OPTIONS}")
target_compile_definitions(${PROJECT_NAME} PRIVATE ${RSP_GCC_STRICT_COMPILE_DEFINITIONS})

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
