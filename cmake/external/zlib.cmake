# -------------------------------------------------------------------------------------------------------------- #
# ZLib
# @see https://www.zlib.net/
# @see https://github.com/madler/zlib
#
# @see https://cmake.org/cmake/help/latest/module/FindZLIB.html
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing ZLib")

find_package(ZLIB REQUIRED)

# Debug
message(STATUS "Found ZLib v${ZLIB_VERSION}")

# Include & link...
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${ZLIB_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
list(APPEND BUILD_OPTIONS "-DUSE_ZLIB")
