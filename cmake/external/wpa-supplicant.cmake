# -------------------------------------------------------------------------------------------------------------- #
# WPA Supplicant
# @see https://trac.netlabs.org/ports/wiki/wpa_supplicant
#
# @see https://packages.debian.org/sid/libwpa-client-dev
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing WPA Supplicant")

# NOTE: This is a runtime dependency that SHOULD be installed on the system
#       sudo apt install libwpa-client-dev

# find_package(WPA_SUPPLICANT REQUIRED) # NOT going to work without a custom `FindWPASupplicant` module

find_library(wpa_supplicant NAMES "wpa_client" REQUIRED DOC " RSP Core Lib - Path to WPA Supplicant Library (libwpa-client-dev)")

# Debug
if (DEFINED "wpa_supplicant-NOTFOUND")
    message(STATUS "WPA Supplicant (${COLOR_RED}${TEXT_ITALIC}not found${RESTORE})")
else ()
    message(STATUS "Using WPA Supplicant (${COLOR_WHITE}${TEXT_ITALIC}${wpa_supplicant}${RESTORE})")
endif ()

# Include & link...
target_link_libraries(${PROJECT_NAME} wpa_client)

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
