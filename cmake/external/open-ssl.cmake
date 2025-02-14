# -------------------------------------------------------------------------------------------------------------- #
# OpenSSL
# @see https://www.openssl.org/
#
# @see https://cmake.org/cmake/help/latest/module/FindOpenSSL.html
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing OpenSSL")

find_package(OpenSSL REQUIRED)

# Debug
message(STATUS "Using OpenSSL v${OPENSSL_VERSION}")

# Include & link...
#target_include_directories(${PROJECT_NAME}
#    PRIVATE
#        ${OPENSSL_INCLUDE_DIR}
#)

# target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARIES})

# NOTE: Only link to the Crypto library for this project.
target_link_libraries(${PROJECT_NAME} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
list(APPEND BUILD_OPTIONS "-DUSE_OPENSSL")
