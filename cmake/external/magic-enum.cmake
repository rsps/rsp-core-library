# -------------------------------------------------------------------------------------------------------------- #
# Magic Enum C++ Fonts
# @see https://github.com/Neargye/magic_enum
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing Magic Enum C++")

set(MAGIC_ENUM_VERSION "0.9.7")
CPMAddPackage(
    NAME "magic_enum"
    GITHUB_REPOSITORY "Neargye/magic_enum"
    GIT_TAG "v${MAGIC_ENUM_VERSION}"
)

# Debug
message(STATUS "Using Magic Enum C++ v${MAGIC_ENUM_VERSION}")

# Include & link...
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${magic_enum_SOURCE_DIR}/include
)

#target_link_libraries(${PROJECT_NAME} magic_enum::magic_enum)

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
