# -------------------------------------------------------------------------------------------------------------- #
# @deprecated - Replaced by "external/mbed-tls.cmake" module.
#
# @see dependencies.cmake for additional details
# -------------------------------------------------------------------------------------------------------------- #

include(FetchContent)
find_package(Git REQUIRED)

FetchContent_Declare(
        mbedtls
        GIT_REPOSITORY "https://github.com/Mbed-TLS/mbedtls.git"
        GIT_TAG "v3.6.0"
        GIT_PROGRESS ON
        USES_TERMINAL_DOWNLOAD ON
)
FetchContent_MakeAvailable(mbedtls)
