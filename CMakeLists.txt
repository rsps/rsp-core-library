cmake_minimum_required (VERSION 3.16)

# https://cliutils.gitlab.io/modern-cmake/modern-cmake.pdf

# Usage: cmake [-DRELEASE_BUILD=ON] [-DPLATFORM_P05=ON] [FREETYPE_FONTS=OFF] [-DOPENSSL_CRYPTO=OFF] ..

OPTION(RELEASE_BUILD "Set to turn off debug output" OFF) # Disabled by default.
OPTION(PLATFORM_P05 "Set to build for P05 ARM platform" OFF) # Disabled by default.
OPTION(FREETYPE_FONTS "Build fonts driver for the FreeType engine." ON) # Enabled by default.
OPTION(OPENSSL_CRYPTO "Build with OpenSSL encryption engine." ON) # Enabled by default.

if(PLATFORM_P05)
    set (CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ToolchainFile.txt)
endif()

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

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(rsp-core-lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
        ${CMAKE_CURRENT_BINARY_DIR}/src
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
    -DDEBUG_LOG)
endif()

add_subdirectory(src)

#--------------------------------------------------------
# Rules to make tests
#-----------------------

# Include external projects Doctest from github.
 
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    Doctest
    
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext-libs/doctest"
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    GIT_TAG "2.4.6"
    TIMEOUT 10

    UPDATE_COMMAND ${GIT_EXECUTABLE} checkout ${GIT_TAG}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)
ExternalProject_Get_Property(Doctest source_dir)
set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")

enable_testing()

add_executable("rsp-core-lib-test")
add_subdirectory(tests)
add_dependencies("rsp-core-lib-test" Doctest)
target_include_directories("rsp-core-lib-test"
    PUBLIC
        ${DOCTEST_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        src
)
# Disable shadow warning to silence doctest...
target_compile_options(rsp-core-lib-test PRIVATE
    ${GOOD_COMPILER_OPTIONS}
    ${FREETYPE_BUILD_OPTIONS}
    ${OPENSSL_BUILD_OPTIONS}
)

if (NOT RELEASE_BUILD)
target_compile_options(rsp-core-lib-test PRIVATE 
    -DGFX_FPS=10)
endif()

#get_target_property(INCLUDE_DIRS "rsp-core-lib-test" INCLUDE_DIRECTORIES)
#message(STATUS "IncDirs: ${INCLUDE_DIRS}")

target_link_libraries ("rsp-core-lib-test"
    rsp-core-lib
    Threads::Threads
    ${FONT_LIB}
    ${CRYPTO_LIB}
)
#target_link_libraries(demo ${FREETYPE_LIBRARIES})

add_test("tests" "rsp-core-lib-test")
