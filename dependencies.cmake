include_guard(GLOBAL)

function(rsp_core_add_dependencies ATARGET)
    message(NOTICE "Installing dependencies for ${ATARGET}")

    include(FetchContent)

    # --------------------------------------------------------------------------------------------------------------

    if(CMAKE_COMPILER_IS_GNUCC)
        # For std::stacktrace: link against libstdc++exp for GCC 14 or greater,
        # or libstdc++_libbacktrace for GCC 12-13.
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
            target_link_libraries(${ATARGET} PUBLIC stdc++exp)
        elseif(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
            target_link_libraries(${ATARGET} PUBLIC stdc++_libbacktrace)
        endif()
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(GFX)
        target_compile_options(${ATARGET} PUBLIC -DUSE_GFX)

        if(SDL2)
            find_package(SDL2 REQUIRED)
            target_link_libraries(${ATARGET} PUBLIC SDL2::SDL2)
            target_compile_options(${ATARGET} PUBLIC -DUSE_GFX_SDL)
        else()
            target_compile_options(${ATARGET} PUBLIC -DUSE_GFX_SW)
        endif()

        if(FREETYPE_FONTS)
            find_package(Freetype REQUIRED)
            target_link_libraries(${ATARGET} PUBLIC Freetype::Freetype)
            target_compile_options(${ATARGET} PUBLIC -DUSE_FREETYPE -DFT_CONFIG_OPTION_ERROR_STRINGS)
        endif()
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(SYSLOG)
        target_compile_options(${ATARGET} PUBLIC -DSYSLOG)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum
        GIT_TAG v0.9.8
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(magic_enum)
    target_link_libraries(${ATARGET} PUBLIC magic_enum::magic_enum)

    # --------------------------------------------------------------------------------------------------------------

    if(NET_LIBCURL)
        find_package(CURL 7.33 REQUIRED)
        target_link_libraries(${ATARGET} PUBLIC CURL::libcurl)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(NET_WPA_SUPPLICANT)
        find_package(wpa_supplicant REQUIRED)
        target_link_libraries(${ATARGET} PUBLIC wpa_supplicant::wpa_supplicant)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(OPENSSL_CRYPTO)
        find_package(OpenSSL REQUIRED)
        target_link_libraries(${ATARGET} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
        target_compile_options(${ATARGET} PUBLIC -DUSE_OPENSSL)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(MBEDTLS_CRYPTO)
        # Abort if OpenSSL option is enabled.
        if(OPENSSL_CRYPTO)
            message(FATAL_ERROR "Unable to build using MBed TLS, because `OPENSSL_CRYPTO` option is enabled.")
        endif()

        if(NOT ESP_PLATFORM)
            FetchContent_Declare(
                mbedtls
                GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls
                GIT_TAG v3.6.5
                GIT_SHALLOW TRUE
            )
            set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)  # Disable building Mbed-TLS's own tests
            set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
            FetchContent_MakeAvailable(mbedtls)
            target_link_libraries(${ATARGET} PUBLIC MbedTLS::mbedtls MbedTLS::mbedcrypto MbedTLS::mbedx509)
        endif()

        target_compile_options(${ATARGET} PUBLIC -DUSE_MBEDTLS)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(ZLIB)
        find_package(ZLIB REQUIRED)
        target_link_libraries(${ATARGET} PUBLIC ZLIB::ZLIB)
        target_compile_options(${ATARGET} PUBLIC -DUSE_ZLIB)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(STD_THREAD)
        set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
        set(THREADS_PREFER_PTHREAD_FLAG TRUE)
        find_package(Threads REQUIRED)
        target_link_libraries(${ATARGET} PUBLIC Threads::Threads)
        target_compile_options(${ATARGET} PUBLIC -DUSE_STD_THREAD)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    # ...etc

endfunction()
