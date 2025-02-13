# -------------------------------------------------------------------------------------------------------------- #
# FreeType Fonts
# @see https://freetype.org/
#
# @see https://cmake.org/cmake/help/latest/module/FindFreetype.html
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing Magic Enum C++")

set(MAGIC_ENUM_VERSION "0.8.2")
CPMAddPackage(
    NAME "magic_enum"
    GITHUB_REPOSITORY "Neargye/magic_enum"
    GIT_TAG "v${MAGIC_ENUM_VERSION}"
)

# Debug
message(VERBOSE "Magic Enum C++ v${MAGIC_ENUM_VERSION} installed")

# Include & link...
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${magic_enum_SOURCE_DIR}/include
)

#target_link_libraries(${PROJECT_NAME} magic_enum::magic_enum)

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
