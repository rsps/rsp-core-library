# -------------------------------------------------------------------------------------------------------------- #
# MBed TLS
# @see https://www.trustedfirmware.org/projects/mbed-tls/
# @see https://github.com/Mbed-TLS/mbedtls
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing MBed TLS")

set(MBED_TLS_VERSION "3.6.0")
CPMAddPackage(
    NAME "mbedtls"
    GITHUB_REPOSITORY "Mbed-TLS/mbedtls"
    GIT_TAG "v${MBED_TLS_VERSION}"
)

# Debug
message(VERBOSE "MBed TLS v${MBED_TLS_VERSION} installed")

# Include & link...

# NOTE: how to include libraries for v3.x
#target_link_libraries(${PROJECT_NAME}
#    PUBLIC
#        MbedTLS::mbedtls
#        MbedTLS::mbedcrypto
#        MbedTLS::mbedx509
#)

# NOTE: how to include libraries for future releases (v4.x)
# @see https://github.com/Mbed-TLS/mbedtls?tab=readme-ov-file#consuming-mbed-tls
#target_link_libraries(${PROJECT_NAME}
#    PUBLIC
#        MbedTLS::mbedtls
#        MbedTLS::tfpsacrypto
#        MbedTLS::mbedx509
#)

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
