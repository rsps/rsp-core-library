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
OPTION(NET_LIBCURL "Build with LibCurl network library." ON) # Enabled by default.
OPTION(BUILD_TESTING "Set to build test binaries" OFF) # Enabled by default.
OPTION(ZLIB "Build with ZLib Inflate/Deflate compression." OFF)
OPTION(NET_WPA_SUPPLICANT "Build with libwpa-client wpa_supplicant library." ON) # Enabled by default.
OPTION(STD_THREAD "Thread architecture to use." ON) # C++ std::thread
OPTION(SYSLOG "Include syslog interface" ON)

# -------------------------------------------------------------------------------------------------------------- #
# Setup
# -------------------------------------------------------------------------------------------------------------- #

# Install "CMake Scripts" dependency
include(FetchContent)
FetchContent_Declare("rsp-cmake-scripts" GIT_REPOSITORY "https://github.com/rsps/cmake-scripts" GIT_TAG "v0.1.0")
FetchContent_MakeAvailable("rsp-cmake-scripts")

# Abort if building in-source
include("rsp/helpers")
fail_in_source_build()

# Append this project's local module paths.
list(FIND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" has_rsp_core_lib_module_path)
if(has_rsp_core_lib_module_path EQUAL -1)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
endif()

# Set toolchain file, if needed.
if(ARCH_ARM)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ToolchainFile.txt")
endif()

# Obtain GCC info.
# TODO: This part here SHOULD be extracted into util in our "CMake Scripts"
find_program(GCC_TOOL NAMES g++-latest g++-HEAD g++-14 g++-13 g++-12 g++-11)
execute_process(COMMAND ${GCC_TOOL} --version OUTPUT_VARIABLE GCC_TOOL_VERSION ERROR_VARIABLE GCC_TOOL_VERSION)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" GCC_TOOL_VERSION "${GCC_TOOL_VERSION}")

message(STATUS "🔖 GCC ${GCC_TOOL_VERSION} (${GCC_TOOL})")

# Define CPP standards, ...etc
set(CMAKE_CXX_STANDARD 23)

# -------------------------------------------------------------------------------------------------------------- #
# Project
# -------------------------------------------------------------------------------------------------------------- #

project (rsp-core-lib
    DESCRIPTION "Open-source library for medical device software."
    HOMEPAGE_URL "https://github.com/rsps/rsp-core-library"
)

# Declare empty lists
set(BUILD_OPTIONS "")
set(LIBRARIES "")
if ("${GCC_TOOL}" MATCHES "\/g\\+\\+-1[3-9]+$")
    list(APPEND LIBRARIES "-lstdc++exp")
elseif ("${GCC_TOOL}" MATCHES "\/g\\+\\+-12$")
    list(APPEND LIBRARIES "-lstdc++_libbacktrace")
endif()

# -------------------------------------------------------------------------------------------------------------- #
# Dependencies
# -------------------------------------------------------------------------------------------------------------- #

#include(FetchContent)
find_package(Git REQUIRED)

if (GFX)
    list(APPEND BUILD_OPTIONS -DUSE_GFX)

    if (SDL2)
        find_package(SDL2 REQUIRED)
        list(APPEND BUILD_OPTIONS -DUSE_GFX_SDL)
        list(APPEND LIBRARIES -lSDL2)
    else()
        list(APPEND BUILD_OPTIONS -DUSE_GFX_SW)
    endif()

    if (FREETYPE_FONTS)
        find_package(Freetype REQUIRED)
        list(APPEND BUILD_OPTIONS -DUSE_FREETYPE -DFT_CONFIG_OPTION_ERROR_STRINGS)
        list(APPEND LIBRARIES Freetype::Freetype)
    endif()
endif()

if (SYSLOG)
    list(APPEND BUILD_OPTIONS -DSYSLOG)
endif ()

# Include external project magic_enum from github.
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY "https://github.com/Neargye/magic_enum"
    GIT_TAG "v0.8.2"
)
FetchContent_MakeAvailable(magic_enum)

if (NET_LIBCURL)
#    set(CURL_LIBRARY "-lcurl")
    find_package(CURL 7.33 REQUIRED)
    list(APPEND LIBRARIES CURL::libcurl)
#    include_directories(${CURL_INCLUDE_DIR})
endif()

if (NET_WPA_SUPPLICANT)
    #sudo apt install libwpa-client-dev
    #find_package(WPA_SUPPLICANT REQUIRED)
    list(APPEND LIBRARIES wpa_client)
endif()

if (OPENSSL_CRYPTO)
    find_package(OpenSSL REQUIRED)
    list(APPEND LIBRARIES OpenSSL::Crypto)
    list(APPEND BUILD_OPTIONS -DUSE_OPENSSL)
endif ()

if (MBEDTLS_CRYPTO)
    if (NOT ESP_PLATFORM)
        find_package(MbedTLS REQUIRED)
        list(APPEND LIBRARIES MbedTLS::mbedcrypto)
    endif()
    list(APPEND BUILD_OPTIONS -DUSE_MBEDTLS)
endif()

if (ZLIB)
    find_package(ZLIB REQUIRED)
    list(APPEND BUILD_OPTIONS -DUSE_ZLIB)
    list(APPEND LIBRARIES ${ZLIB_LIBRARIES})
endif()

#message(STATUS "BinDir: ${CMAKE_CURRENT_BINARY_DIR}")

if (STD_THREAD)
    set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set (THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)
    list(APPEND BUILD_OPTIONS -DUSE_STD_THREAD)
    list(APPEND LIBRARIES Threads::Threads)
endif ()

add_library("${PROJECT_NAME}" STATIC)

# Must use GNUInstallDirs to install libraries into correct locations on all platforms.
include(GNUInstallDirs)
target_link_libraries("${PROJECT_NAME}"
    ${LIBRARIES}
)

# -------------------------------------------------------------------------------------------------------------- #
# Targets & Compile Options
# -------------------------------------------------------------------------------------------------------------- #

# Keep symbol information for exception stack traces
SET (CMAKE_ENABLE_EXPORTS TRUE)

# Define headers for this library. PUBLIC headers are used for compiling the library, and will be added to
# consumers' build paths.
target_include_directories("${PROJECT_NAME}"
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${magic_enum_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_BINARY_DIR}/src
        ${FREETYPE_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
        ${SDL_INCLUDE_DIRS}
)

include("cmake/strict-compile-options.cmake")
if (NOT RELEASE_BUILD)
    add_compile_options(${STRICT_COMPILE_OPTIONS})
endif()

target_compile_options("${PROJECT_NAME}" PUBLIC
    -Wshadow
    ${BUILD_OPTIONS}
)

if (NOT RELEASE_BUILD)
target_compile_options("${PROJECT_NAME}" PRIVATE
    ${STRICT_COMPILE_OPTIONS}
    -DDEBUG_LOG
    -g
    -O0
)
else()
target_compile_options("${PROJECT_NAME}" PRIVATE
    -O3
)
endif()

add_subdirectory(src)

# -------------------------------------------------------------------------------------------------------------- #
# Tests
# -------------------------------------------------------------------------------------------------------------- #

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

# -------------------------------------------------------------------------------------------------------------- #
# Misc.
# -------------------------------------------------------------------------------------------------------------- #

# Resources:
# https://cliutils.gitlab.io/modern-cmake/modern-cmake.pdf
