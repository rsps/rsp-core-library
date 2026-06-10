/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_SECURITY_MBEDTLS_TLS_SOCKET_H
#define RSP_CORE_LIB_INCLUDE_SECURITY_MBEDTLS_TLS_SOCKET_H

#include "exceptions.h"
#include <rsp/logging/LogChannel.h>
#include <rsp/security/ITLSSocket.h>
#include <span>

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
#endif
    #include <mbedtls/net_sockets.h>
    #include <mbedtls/ssl.h>
    #include <mbedtls/entropy.h>
    #include <mbedtls/ctr_drbg.h>
    #include <mbedtls/debug.h>
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif

namespace rsp::security {


class TLSSocket : public ITLSSocket, public logging::NamedLogChannel
{
public:
    explicit TLSSocket(network::ConnectionOptions aOptions);

    TLSSocket& SetSocket(posix::Socket &arSocket) override;
    TLSSocket& Close() override;

    size_t Write(std::span<std::byte const> aData) override;
    size_t Read(std::span<std::byte> aData) override;

protected:
    struct tlsNet : public mbedtls_net_context {
        tlsNet() : mbedtls_net_context{} { mbedtls_net_init( this ); }
        ~tlsNet() { mbedtls_net_free(this); }
    };
    struct tlsEntropy : public mbedtls_entropy_context {
        tlsEntropy() : mbedtls_entropy_context{} { mbedtls_entropy_init( this ); }
        ~tlsEntropy() { mbedtls_entropy_free(this); }
    };
    struct tlsSSL : public mbedtls_ssl_context {
        tlsSSL() : mbedtls_ssl_context{} { mbedtls_ssl_init(this); }
        ~tlsSSL() { mbedtls_ssl_free(this); }
    };
    struct tlsConfig : public mbedtls_ssl_config {
        tlsConfig() : mbedtls_ssl_config{} { mbedtls_ssl_config_init(this); }
        ~tlsConfig() { mbedtls_ssl_config_free(this); }
    };
    struct tlsX509 : public mbedtls_x509_crt {
        tlsX509() : mbedtls_x509_crt{} { mbedtls_x509_crt_init(this); }
        ~tlsX509() { mbedtls_x509_crt_free(this); }
    };
    struct tlsPrivateKey : public mbedtls_pk_context {
        tlsPrivateKey() : mbedtls_pk_context{} { mbedtls_pk_init(this); }
        ~tlsPrivateKey() { mbedtls_pk_free(this); }
    };
    struct tlsRandom : public mbedtls_ctr_drbg_context {
        tlsRandom(tlsEntropy& arEntropy, const security::SecureBuffer &arNonce);
        ~tlsRandom() { mbedtls_ctr_drbg_free(this); }
   };

    tlsNet mNet{};
    tlsEntropy mEntropy{};
    tlsSSL mSsl{};
    tlsConfig mConfig{};
    tlsRandom mRandom;
    tlsX509 mCaChain{};
    tlsX509 mClientCert{};
    tlsPrivateKey mPrivateKey{};

    static void debugLog(void *ctx, int level, const char *file, int line, const char *str);
    static int rng_get(void *p_rng, unsigned char *output, size_t output_len);
    /**
     * \brief Handle results from many SSL functions.
     * \param aErr Negative integer result from operation
     * \return True if result is ending operation. False to continue.
     * \throws EMbedTLSError derivatives if any error is detected
     */
    bool resultHandler(int aErr);
};

} // rsp::security

#endif //RSP_CORE_LIB_INCLUDE_SECURITY_MBEDTLS_TLS_SOCKET_H
