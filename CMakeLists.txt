# -------------------------------------------------------------------------------------------------------------- #
# RSP Core Library
# -------------------------------------------------------------------------------------------------------------- #

cmake_minimum_required (VERSION 3.30)

# -------------------------------------------------------------------------------------------------------------- #
# Options
# -------------------------------------------------------------------------------------------------------------- #

# Usage: cmake [-DRELEASE_BUILD=ON] [-DPLATFORM_P05=ON] [FREETYPE_FONTS=OFF] [-DOPENSSL_CRYPTO=OFF] ..

OPTION(RELEASE_BUILD "Set to turn off debug output" OFF) # Disabled by default.
OPTION(ARCH_ARM "Set to cross-compile for ARM CPU" OFF) # Disabled by default.
OPTION(GFX "Include graphic components." ON)
OPTION(SDL2 "Use SDL2 for graphic acceleration." ON)
OPTION(FREETYPE_FONTS "Build fonts driver for the FreeType engine." ON) # Enabled by default.
OPTION(OPENSSL_CRYPTO "Build with OpenSSL encryption engine." ON) # Enabled by default.
OPTION(MBEDTLS_CRYPTO "Build with Mbed TLS encryption engine." OFF) # Disabled by default.
OPTION(NET_LIBCURL "Build with LibCurl network library." OFF) # Disabled by default.
OPTION(NET_EMBED_HTTP "Build with Embed HTTP network library." ON) # Enabled by default.
OPTION(BUILD_TESTING "Set to build test binaries" OFF) # Enabled by default.
OPTION(ZLIB "Build with ZLib Inflate/Deflate compression." OFF)
OPTION(NET_WPA_SUPPLICANT "Build with libwpa-client wpa_supplicant library." ON) # Enabled by default.
OPTION(STD_THREAD "Thread architecture to use." ON) # C++ std::thread
OPTION(SYSLOG "Include syslog interface" ON)

# -------------------------------------------------------------------------------------------------------------- #
# Setup
# -------------------------------------------------------------------------------------------------------------- #

find_package(Git REQUIRED)

# Install "CMake Scripts" dependency
include(FetchContent)
FetchContent_Declare("rsp-cmake-scripts" GIT_REPOSITORY "https://github.com/rsps/cmake-scripts" GIT_TAG "v0.4.0")
FetchContent_MakeAvailable("rsp-cmake-scripts")

# Abort if building in-source
include("rsp/helpers")
fail_in_source_build()

# Append this project's local module paths.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set toolchain file, if needed.
if(ARCH_ARM)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ToolchainFile.txt")
endif()

# Obtain GCC info.
include("rsp/compiler")
gcc_info(OUTPUT GCC_TOOL)

# Define CPP standards, ...etc
set(CMAKE_CXX_STANDARD 23)

# -------------------------------------------------------------------------------------------------------------- #
# Project
# -------------------------------------------------------------------------------------------------------------- #

# Get project's version from file
include("setup-version")

project (rsp-core-lib
    VERSION "${version_VERSION}"
    DESCRIPTION "Open-source library for medical device software."
    HOMEPAGE_URL "https://github.com/rsps/rsp-core-library"
)

# Add this project as "library". This MUST be declared such, or dependencies
# will not be added or linked correctly!
add_library("${PROJECT_NAME}" STATIC)

# Debug
message("${COLOR_GREEN}${TEXT_BOLD}Building${COLOR_DEFAULT} ${PROJECT_NAME}${TEXT_BOLD_RESTORE}")
message(STATUS "Using CMAKE (${COLOR_WHITE}${TEXT_ITALIC}${CMAKE_VERSION}${RESTORE})")
message(STATUS "Using GCC ${GCC_TOOL_VERSION} (${COLOR_WHITE}${TEXT_ITALIC}${GCC_TOOL}${RESTORE})")

# -------------------------------------------------------------------------------------------------------------- #
# Dependencies
# -------------------------------------------------------------------------------------------------------------- #

# List of build options that dependencies can append to.
set(BUILD_OPTIONS "")

include("dependencies.cmake")

# Debug
# dump(BUILD_OPTIONS)

# -------------------------------------------------------------------------------------------------------------- #
# Targets & Compile Options
# -------------------------------------------------------------------------------------------------------------- #

# Keep symbol information for exception stack traces
set(CMAKE_ENABLE_EXPORTS TRUE)

# Define headers for this library. PUBLIC headers are used for compiling the library, and will be added to
# consumers' build paths.
target_include_directories("${PROJECT_NAME}"
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${PROJECT_BINARY_DIR}/src
)

# Configure compile options for this project
set(RSP_CORE_LIB_COMPILE_OPTIONS "${RSP_GCC_STRICT_COMPILE_OPTIONS}")
list(REMOVE_ITEM RSP_CORE_LIB_COMPILE_OPTIONS "-Wswitch-default")

if (NOT RELEASE_BUILD)
    add_compile_options("${RSP_CORE_LIB_COMPILE_OPTIONS}")
endif()

target_compile_options("${PROJECT_NAME}" PUBLIC
    -Wshadow
    ${BUILD_OPTIONS}
)

if (NOT RELEASE_BUILD)
    target_compile_options("${PROJECT_NAME}" PRIVATE
            "${RSP_CORE_LIB_COMPILE_OPTIONS}"
            -DDEBUG_LOG
            -g
            -O0
    )
else()
    target_compile_options("${PROJECT_NAME}" PRIVATE
        -O3
    )
endif()

if (EHTTP_REQUEST_BUFFER_SIZE)
    message(STATUS "Compiling eHttp with request buffer size: ${EHTTP_REQUEST_BUFFER_SIZE}")
    target_compile_options("${PROJECT_NAME}" PRIVATE
        -DEHTTP_REQUEST_BUFFER_SIZE=${EHTTP_REQUEST_BUFFER_SIZE}
    )
endif ()

add_subdirectory(src)

# -------------------------------------------------------------------------------------------------------------- #
# Tests
# -------------------------------------------------------------------------------------------------------------- #

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (BUILD_TESTING)
    # Define "test binary" such that dev-dependencies can include or link
    # resources for it...
    set(TEST_BINARY "rsp-core-lib-test")
    add_executable(${TEST_BINARY})

    include("dev-dependencies.cmake")

    add_subdirectory(tests)
endif()

# -------------------------------------------------------------------------------------------------------------- #
# Misc.
# -------------------------------------------------------------------------------------------------------------- #

# Resources:
# https://cliutils.gitlab.io/modern-cmake/modern-cmake.pdf
