# Findwpa_supplicant.cmake
# ------------------------
# Find the wpa_supplicant client library and headers.
#
# Result Variables:
#
#   wpa_supplicant_FOUND        - System has wpa_supplicant
#   wpa_supplicant_INCLUDE_DIRS - The wpa_supplicant include directories
#   wpa_supplicant_LIBRARIES    - The libraries needed to use wpa_supplicant
#
# Imported Targets:
#
#   wpa_supplicant::wpa_supplicant - Target for wpa_supplicant library
#

find_path(wpa_supplicant_INCLUDE_DIR
    NAMES wpa_ctrl.h
    PATH_SUFFIXES wpa_supplicant
)
mark_as_advanced(wpa_supplicant_INCLUDE_DIR)

find_library(wpa_supplicant_LIBRARY
    NAMES wpa_client
)
mark_as_advanced(wpa_supplicant_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wpa_supplicant
    DEFAULT_MSG
    wpa_supplicant_LIBRARY wpa_supplicant_INCLUDE_DIR
)

if(wpa_supplicant_FOUND)
    set(wpa_supplicant_LIBRARIES ${wpa_supplicant_LIBRARY})
    set(wpa_supplicant_INCLUDE_DIRS ${wpa_supplicant_INCLUDE_DIR})

    if(NOT TARGET wpa_supplicant::wpa_supplicant)
        if(wpa_supplicant_LIBRARY MATCHES "\\.(a|lib)$")
            add_library(wpa_supplicant::wpa_supplicant STATIC IMPORTED)
        else()
            add_library(wpa_supplicant::wpa_supplicant SHARED IMPORTED)
            set_target_properties(wpa_supplicant::wpa_supplicant PROPERTIES
                IMPORTED_NO_SONAME TRUE # libwpa_client.so lacks SONAME in its ELF headers
            )
        endif()
        set_target_properties(wpa_supplicant::wpa_supplicant PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${wpa_supplicant_INCLUDE_DIR}"
            IMPORTED_LOCATION "${wpa_supplicant_LIBRARY}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        )
    endif()
endif()
