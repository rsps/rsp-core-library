cmake_minimum_required (VERSION 3.16)

# https://cliutils.gitlab.io/modern-cmake/modern-cmake.pdf

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

#set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
#set (THREADS_PREFER_PTHREAD_FLAG TRUE)
#find_package(Threads REQUIRED)

set (CMAKE_CXX_STANDARD 20)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)


#message(STATUS "BinDir: ${CMAKE_CURRENT_BINARY_DIR}")

add_library("rsp-core-lib" STATIC)
add_subdirectory(src)

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(rsp-core-lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/graphics>
#        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/graphics/primitives>
#        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/graphics/controls>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
        ${CMAKE_CURRENT_BINARY_DIR}/src
)
add_compile_options(-Wall -Wextra -Wold-style-cast -pedantic -Werror)

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
    TIMEOUT 10

    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
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

#get_target_property(INCLUDE_DIRS "rsp-core-lib-test" INCLUDE_DIRECTORIES)
#message(STATUS "IncDirs: ${INCLUDE_DIRS}")

target_link_libraries ("rsp-core-lib-test"
    rsp-core-lib
    #Threads::Threads
)

add_test("tests" "rsp-core-lib-test")
