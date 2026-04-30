# -------------------------------------------------------------------------------------------------------------- #
# Dependencies
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

include("rsp/debug")
include("rsp/logging")

macro(install_dependencies)
    message(NOTICE "${COLOR_WHITE}Installing Dependencies for${RESTORE} ${PROJECT_NAME}")

    # Avoid building tests for dependencies...
    # TODO: Bug - for some reason, safeguard_properties() fails to restore BUILD_TESTING ... what the hell!
    # set(BUILD_TESTING off)

    # Ensure predefined directories are defined
    include(GNUInstallDirs)

    # -------------------------------------------------------------------------------------------------------------- #

    if ("${GCC_TOOL}" MATCHES "\/g\\+\\+-1[3-9]+$")
        message(STATUS "Using stdc++exp")

        target_link_libraries("${PROJECT_NAME}" "-lstdc++exp")
    elseif ("${GCC_TOOL}" MATCHES "\/g\\+\\+-12$")
        message(STATUS "Using stdc++_libbacktrace")

        target_link_libraries("${PROJECT_NAME}" "-lstdc++_libbacktrace")
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    if (GFX)
        list(APPEND BUILD_OPTIONS "-DUSE_GFX")

        if (SDL2)
            include("external/sdl2")
        else ()
            list(APPEND BUILD_OPTIONS "-DUSE_GFX_SW")
        endif ()

        if (FREETYPE_FONTS)
            include("external/freetype")
        endif ()
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    if (SYSLOG)
        list(APPEND BUILD_OPTIONS -DSYSLOG)
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    include("external/magic-enum")

    # -------------------------------------------------------------------------------------------------------------- #

    if (NET_LIBCURL)
        include("external/curl")
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    if (NET_WPA_SUPPLICANT)
        include("external/wpa-supplicant")
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    if (OPENSSL_CRYPTO)
        include("external/open-ssl")
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    if (MBEDTLS_CRYPTO)
        # Abort if OpenSSL option is enabled.
        if (OPENSSL_CRYPTO)
            error("Unable to build using MBed TLS, because `OPENSSL_CRYPTO` option is enabled.")
        endif ()

        if (NOT ESP_PLATFORM)
            include("external/mbed-tls")
        endif ()

        list(APPEND BUILD_OPTIONS "-DUSE_MBEDTLS")
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    if (ZLIB)
        include("external/zlib")
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    if (STD_THREAD)
        include("external/threads")
    endif ()

    # -------------------------------------------------------------------------------------------------------------- #

    # ...etc

endmacro()
safeguard_properties("install_dependencies" "BUILD_TESTING")
