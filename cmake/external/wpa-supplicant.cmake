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

find_package(wpa_supplicant REQUIRED)

target_link_libraries(${PROJECT_NAME} wpa_supplicant::wpa_supplicant)

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
