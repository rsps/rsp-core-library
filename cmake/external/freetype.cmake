# -------------------------------------------------------------------------------------------------------------- #
# FreeType Fonts
# @see https://freetype.org/
#
# @see https://cmake.org/cmake/help/latest/module/FindFreetype.html
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing FreeType")

find_package(Freetype REQUIRED)

# Debug
message(STATUS "Using FreeType v${FREETYPE_VERSION_STRING}")

# Include & link...
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
list(APPEND BUILD_OPTIONS
    "-DUSE_FREETYPE"
    "-DFT_CONFIG_OPTION_ERROR_STRINGS"
)
