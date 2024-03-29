cmake_minimum_required (VERSION 3.16)

# https://cliutils.gitlab.io/modern-cmake/modern-cmake.pdf

# Usage: cmake [-DRELEASE_BUILD=ON] [-DPLATFORM_P05=ON] [FREETYPE_FONTS=OFF] [-DOPENSSL_CRYPTO=OFF] ..

OPTION(RELEASE_BUILD "Set to turn off debug output" OFF) # Disabled by default.
OPTION(ARC_ARM "Set to cross-compile for ARM CPU" OFF) # Disabled by default.
OPTION(FREETYPE_FONTS "Build fonts driver for the FreeType engine." ON) # Enabled by default.
OPTION(OPENSSL_CRYPTO "Build with OpenSSL encryption engine." ON) # Enabled by default.
OPTION(NET_LIBCURL "Build with LibCurl network library." ON) # Enabled by default.
OPTION(BUILD_TESTING "Set to build test binaries" ON) # Enabled by default.

if(ARCH_ARM)
    set (CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ToolchainFile.txt)
endif()

#set(CMAKE_C_COMPILER "gcc-11")
#set(CMAKE_CXX_COMPILER "g++-11")

find_program(GCC_TOOL NAMES g++-latest g++-HEAD g++-11)
execute_process(COMMAND ${GCC_TOOL} --version OUTPUT_VARIABLE GCC_TOOL_VERSION ERROR_VARIABLE GCC_TOOL_VERSION)
string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" GCC_TOOL_VERSION "${GCC_TOOL_VERSION}")
message(STATUS "🔖 GCC ${GCC_TOOL_VERSION} (${GCC_TOOL})")

project (rsp-core-lib)

set (LIB_VERSION_MAJOR 0)
set (LIB_VERSION_MINOR 1)
set (LIB_VERSION_PATCH 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${PROJECT_SOURCE_DIR}/version-def.h.in"
    "${PROJECT_BINARY_DIR}/src/version-def.h"
)


set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
set (THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set (CMAKE_CXX_STANDARD 20)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

#message(STATUS "BinDir: ${CMAKE_CURRENT_BINARY_DIR}")

add_compile_options(
    -pedantic
    -Wall
    -Wextra
    -Wold-style-cast
    -Wcast-align
    -Wcast-qual
    -Wctor-dtor-privacy
    -Wdisabled-optimization
    -Wformat=2
    -Winit-self
    -Wlogical-op
    -Wmissing-declarations
    -Wmissing-include-dirs
    -Wnoexcept
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-conversion
    -Wsign-promo
    -Wstrict-null-sentinel
    -Wstrict-overflow=3
#    -Wswitch-default
    -Wundef
    -Wno-unused
    -Weffc++
    -Werror
#     -v -dD
)

include(FetchContent)
find_package(Git REQUIRED)

# Include external projects Doctest from github.
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY "https://github.com/Neargye/magic_enum"
    GIT_TAG "v0.8.2"
)
FetchContent_MakeAvailable(magic_enum)

add_library("rsp-core-lib" STATIC)

if (FREETYPE_FONTS)
    find_package(Freetype REQUIRED)
    set(FREETYPE_BUILD_OPTIONS -DUSE_FREETYPE -DFT_CONFIG_OPTION_ERROR_STRINGS)
    set(FONT_LIB Freetype::Freetype)
endif()

if (NET_LIBCURL)
#    set(CURL_LIBRARY "-lcurl") 
    find_package(CURL 7.33 REQUIRED)
    set(LIBCURL_BUILD_OPTIONS -DUSE_CURL)
    set(NETWORK_LIB CURL::libcurl)
#    include_directories(${CURL_INCLUDE_DIR})
endif()

if (OPENSSL_CRYPTO)
    find_package(OpenSSL REQUIRED)
    set(OPENSSL_BUILD_OPTIONS -DUSE_OPENSSL)
    set(CRYPTO_LIB OpenSSL::Crypto)
endif()

target_link_libraries ("rsp-core-lib"
    ${FONT_LIB}
    ${CRYPTO_LIB}
    ${NETWORK_LIB}
)

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(rsp-core-lib
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${PROJECT_BINARY_DIR}/src
        ${FREETYPE_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
        ${magic_enum_SOURCE_DIR}/include
)

target_compile_options(rsp-core-lib PUBLIC
    -Wshadow
    ${FREETYPE_BUILD_OPTIONS}
    ${OPENSSL_BUILD_OPTIONS}
    ${LIBCURL_BUILD_OPTIONS}
)

if (NOT RELEASE_BUILD)
target_compile_options(rsp-core-lib PRIVATE
    -DDEBUG_LOG
    -g
    -O0
)
else()
target_compile_options(rsp-core-lib PRIVATE
    -O3
)
endif()

add_subdirectory(src)

#--------------------------------------------------------
# Rules to make tests
#-----------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
