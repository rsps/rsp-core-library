# FindSDL2.cmake
# ---------------
# Find the SDL2 library and headers.
#
# Result Variables:
#
#   SDL2_FOUND        - System has SDL2
#   SDL2_INCLUDE_DIRS - The SDL2 include directories
#   SDL2_LIBRARIES    - The libraries needed to link SDL2
#
# Imported Targets:
#
#   SDL2::SDL2 - Target for SDL2 library
#

# Try config mode first (for when system has SDL2 installed with CMake support)
find_package(SDL2 CONFIG QUIET)

# Fallback if config mode failed
if(NOT SDL2_FOUND)
    find_path(SDL2_INCLUDE_DIR
        NAMES SDL.h
        PATH_SUFFIXES SDL2
    )
    mark_as_advanced(SDL2_INCLUDE_DIR)

    find_library(SDL2_LIBRARY
        NAMES SDL2
    )
    mark_as_advanced(SDL2_LIBRARY)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(SDL2
        DEFAULT_MSG
        SDL2_LIBRARY SDL2_INCLUDE_DIR
    )

    if(SDL2_FOUND)
        set(SDL2_LIBRARIES ${SDL2_LIBRARY})
        set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})

        if(NOT TARGET SDL2::SDL2)
            add_library(SDL2::SDL2 UNKNOWN IMPORTED)
            set_target_properties(SDL2::SDL2 PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
                IMPORTED_LOCATION "${SDL2_LIBRARY}"
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            )
        endif()
    endif()
endif()

