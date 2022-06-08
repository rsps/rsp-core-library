cmake_minimum_required (VERSION 3.16)

# https://cliutils.gitlab.io/modern-cmake/modern-cmake.pdf

# Usage: cmake [-DRELEASE_BUILD=ON] [-DPLATFORM_P05=ON] [FREETYPE_FONTS=OFF] [-DOPENSSL_CRYPTO=OFF] ..

OPTION(RELEASE_BUILD "Set to turn off debug output" OFF) # Disabled by default.
OPTION(PLATFORM_ARM "Set to build for ARM platform" OFF) # Disabled by default.
OPTION(FREETYPE_FONTS "Build fonts driver for the FreeType engine." ON) # Enabled by default.
OPTION(OPENSSL_CRYPTO "Build with OpenSSL encryption engine." ON) # Enabled by default.

if(PLATFORM_ARM)
    set (CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ToolchainFile.txt)
endif()

#set(CMAKE_C_COMPILER "gcc-11")
#set(CMAKE_CXX_COMPILER "g++-11")

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

add_library("rsp-core-lib" STATIC)

if (FREETYPE_FONTS)
    find_package(Freetype REQUIRED)
    set(FREETYPE_BUILD_OPTIONS -DUSE_FREETYPE -DFT_CONFIG_OPTION_ERROR_STRINGS)
    set(FONT_LIB Freetype::Freetype)
endif()

if (OPENSSL_CRYPTO)
    find_package(OpenSSL REQUIRED)
    set(OPENSSL_BUILD_OPTIONS -DUSE_OPENSSL)
    set(CRYPTO_LIB OpenSSL::Crypto)
endif()

target_link_libraries ("rsp-core-lib"
    ${FONT_LIB}
    ${CRYPTO_LIB}
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
)

set(GOOD_COMPILER_OPTIONS -pedantic -Wall -Wextra -Wold-style-cast
    -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization
    -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations
    -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual
    -Wredundant-decls -Wsign-conversion -Wsign-promo
    -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default
    -Wundef -Wno-unused -Weffc++ -Werror
)

target_compile_options(rsp-core-lib PRIVATE
    ${GOOD_COMPILER_OPTIONS}
    -Wshadow
    ${FREETYPE_BUILD_OPTIONS}
    ${OPENSSL_BUILD_OPTIONS}
)

if (NOT RELEASE_BUILD)
target_compile_options(rsp-core-lib PRIVATE
    -DDEBUG_LOG
)
else()
target_compile_options(rsp-core-lib PRIVATE
    -O3 -Wall
)
endif()

add_subdirectory(src)

#--------------------------------------------------------
# Rules to make tests
#-----------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(tests)

