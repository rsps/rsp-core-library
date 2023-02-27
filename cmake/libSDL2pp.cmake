include(FetchContent)
find_package(Git REQUIRED)

# Include C++ wrappers fro SDL2
FetchContent_Declare(
    libSDL2pp
    GIT_REPOSITORY https://github.com/libSDL2pp/libSDL2pp.git
    GIT_TAG        "0.17.0"
)

FetchContent_GetProperties(libSDL2pp)
if(NOT libSDL2pp_POPULATED)
    FetchContent_Populate(libSDL2pp)

    SET(SDL2PP_WITH_IMAGE OFF) # if you need SDL_image support
    SET(SDL2PP_WITH_MIXER OFF) # if you need SDL_mixer support
    SET(SDL2PP_WITH_TTF OFF) # if you need SDL_ttf support
    add_subdirectory(${libsdl2pp_SOURCE_DIR} ${libsdl2pp_BINARY_DIR})
endif()

find_package(SDL2 REQUIRED)
