file(GLOB_RECURSE APPLICATION_FILES application/*.cpp)
file(GLOB_RECURSE COMPRESSION_FILES compression/*.cpp)
file(GLOB_RECURSE EXCEPTIONS_FILES exceptions/*.cpp)
file(GLOB_RECURSE JSON_FILES json/*.cpp)
file(GLOB_RECURSE LOCALIZATION_FILES localization/*.cpp)
file(GLOB_RECURSE LOGGING_FILES logging/*.cpp)
file(GLOB_RECURSE MESSAGING_FILES messaging/*.cpp)
file(GLOB NETWORK_FILES LIST_DIRECTORIES false network/*.cpp)
file(GLOB_RECURSE POSIX_FILES posix/*.cpp)
file(GLOB SECURITY_FILES LIST_DIRECTORIES false security/*.cpp)
file(GLOB_RECURSE UTILS_FILES utils/*.cpp)

if (NET_LIBCURL)
    file(GLOB CURL_FILES network/curl/*.cpp)
    list(APPEND NETWORK_FILES ${CURL_FILES})
endif ()
if (NET_WPA_SUPPLICANT)
    file(GLOB WPA_SUPPLICANT_FILES network/wpa_supplicant/*.cpp)
    list(APPEND NETWORK_FILES ${WPA_SUPPLICANT_FILES})
endif ()

if (GFX)
    file(GLOB_RECURSE GRAPHICS_FILES graphics/*.cpp)
endif ()

if (OPENSSL_CRYPTO)
    file(GLOB OPENSSL_FILES security/openssl/*.cpp)
    list(APPEND SECURITY_FILES ${OPENSSL_FILES})
endif ()


target_sources("rsp-core-lib"
    PRIVATE
        version.cpp
        ${APPLICATION_FILES}
        ${COMPRESSION_FILES}
        ${EXCEPTIONS_FILES}
        ${JSON_FILES}
        ${LOCALIZATION_FILES}
        ${LOGGING_FILES}
        ${MESSAGING_FILES}
        ${POSIX_FILES}
        ${SECURITY_FILES}
        ${UTILS_FILES}
        ${NETWORK_FILES}
        ${GRAPHICS_FILES}
)
