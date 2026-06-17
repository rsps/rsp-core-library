include_guard(GLOBAL)

function(rsp_core_add_dependencies ATARGET)
    message(NOTICE "Installing dependencies for ${ATARGET}")

    include(FetchContent)

    # --------------------------------------------------------------------------------------------------------------

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # Query the libstdc++ version in use (not the GCC/Clang version)
        execute_process(
            COMMAND echo "#include <version>"
            COMMAND ${CMAKE_CXX_COMPILER} -dM -E -x c++ -
            OUTPUT_VARIABLE _CXX_DEFS
        )
        string(REGEX MATCH "#define _GLIBCXX_RELEASE ([0-9]+)" _ "${_CXX_DEFS}")
        set(_GLIBCXX_RELEASE "${CMAKE_MATCH_1}")

        # For std::stacktrace: link against libstdc++exp for GCC/libstdc++ 14 or greater,
        # or libstdc++_libbacktrace for 12-13.
        if(_GLIBCXX_RELEASE GREATER_EQUAL 14)
            target_link_libraries(${ATARGET} PRIVATE stdc++exp)
        elseif(_GLIBCXX_RELEASE GREATER_EQUAL 12)
            target_link_libraries(${ATARGET} PRIVATE stdc++_libbacktrace)
        endif()
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(RSP_CORE_LIB_USE_GFX)
        target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_GFX)

        if(RSP_CORE_LIB_USE_GFX_SDL2)
            find_package(SDL2 REQUIRED)
            target_link_libraries(${ATARGET} PRIVATE SDL2::SDL2)
            target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_GFX_SDL2)
        else()
            set(RSP_CORE_LIB_USE_GFX_SW ON)
            target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_GFX_SW)
        endif()

        if(RSP_CORE_LIB_USE_FREETYPE)
            find_package(Freetype REQUIRED)
            target_link_libraries(${ATARGET} PRIVATE Freetype::Freetype)
            target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_FREETYPE FT_CONFIG_OPTION_ERROR_STRINGS)
        endif()
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(RSP_CORE_LIB_USE_SYSLOG)
        target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_SYSLOG)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(RSP_CORE_LIB_USE_LIBCURL)
        find_package(CURL 7.33 REQUIRED)
        target_link_libraries(${ATARGET} PRIVATE CURL::libcurl)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(RSP_CORE_LIB_USE_WPA_SUPPLICANT)
        find_package(wpa_supplicant REQUIRED)
        target_link_libraries(${ATARGET} PRIVATE wpa_supplicant::wpa_supplicant)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(RSP_CORE_LIB_USE_OPENSSL)
        find_package(OpenSSL REQUIRED)
        target_link_libraries(${ATARGET} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
        target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_OPENSSL)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(RSP_CORE_LIB_USE_MBEDTLS)
        # Abort if OpenSSL option is enabled.
        if(RSP_CORE_LIB_USE_OPENSSL)
            message(FATAL_ERROR "Unable to build using MBed TLS, because `RSP_CORE_LIB_USE_OPENSSL` option is enabled.")
        endif()

        if(NOT ESP_PLATFORM)
            FetchContent_Declare(
                mbedtls
                GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls
                GIT_TAG v3.6.6
                GIT_SHALLOW TRUE
            )
            set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)  # Disable building Mbed-TLS's own tests
            set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
            FetchContent_MakeAvailable(mbedtls)
            target_link_libraries(${ATARGET} PRIVATE MbedTLS::mbedtls MbedTLS::mbedcrypto MbedTLS::mbedx509)
        endif()

        target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_MBEDTLS)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(RSP_CORE_LIB_USE_ZLIB)
        find_package(ZLIB REQUIRED)
        target_link_libraries(${ATARGET} PRIVATE ZLIB::ZLIB)
        target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_ZLIB)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    if(RSP_CORE_LIB_USE_STD_THREAD)
        set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
        set(THREADS_PREFER_PTHREAD_FLAG TRUE)
        find_package(Threads REQUIRED)
        target_link_libraries(${ATARGET} PUBLIC Threads::Threads)
        target_compile_definitions(${ATARGET} PRIVATE RSP_CORE_LIB_USE_STD_THREAD)
    endif()

    # --------------------------------------------------------------------------------------------------------------

    # ...etc

endfunction()
