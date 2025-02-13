# -------------------------------------------------------------------------------------------------------------- #
# SDL
# @see https://www.libsdl.org/
#
# @see https://cmake.org/cmake/help/latest/module/FindSDL.html
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing SDL2")

find_package(SDL2 REQUIRED)

# Debug
message(STATUS "Found SDL2 v${SDL2_VERSION}")

# Include & link...
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
list(APPEND BUILD_OPTIONS "-DUSE_GFX_SDL")
