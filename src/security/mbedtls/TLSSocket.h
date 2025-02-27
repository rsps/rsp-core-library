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

#include <posix/Socket.h>
#include <security/ITLSSocket.h>
#include <span>
#include <string_view>

#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>


namespace rsp::security {


class TLSSocket : public ITLSSocket
{
public:
    explicit TLSSocket(const network::ConnectionOptions& arOptions);
    ~TLSSocket() override;

    TLSSocket& SetSocket(posix::Socket &arSocket) override;
    TLSSocket& Close() override;

    size_t Write(std::span<std::byte const> aData) override;
    size_t Read(std::span<std::byte> aData) override;

protected:
    struct tlsNet : public mbedtls_net_context {
        tlsNet() { mbedtls_net_init( this ); }
        ~tlsNet() { mbedtls_net_free(this); }
    };
    struct tlsEntropy : public mbedtls_entropy_context {
        tlsEntropy() { mbedtls_entropy_init( this ); }
        ~tlsEntropy() { mbedtls_entropy_free(this); }
    };
    struct tlsSSL : public mbedtls_ssl_context {
        tlsSSL() { mbedtls_ssl_init(this); }
        ~tlsSSL() { mbedtls_ssl_free(this); }
    };
    struct tlsConfig : public mbedtls_ssl_config {
        tlsConfig() { mbedtls_ssl_config_init(this); }
        ~tlsConfig() { mbedtls_ssl_config_free(this); }
    };
    struct tlsX509 : public mbedtls_x509_crt {

    };

    struct NetDeleter {
        mbedtls_x509_crt_init( &cacert );
        mbedtls_ctr_drbg_init( &ctr_drbg );

        mbedtls_entropy_init( &entropy );
        if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                           (const unsigned char *) pers,
                                           strlen( pers ) ) ) != 0 )
        {
            printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
            goto exit;
        }

        void operator()(mbedtls_net_context* x) { ::mbedtls_net_free(x); }
    };
    struct SSLDeleter {
        void operator()(mbedtls_ssl_context* x) { ::mbedtls_ssl_free(x); }
    };
    struct ConfigDeleter {
        void operator()(mbedtls_ssl_config* x) { ::mbedtls_ssl_config_free(x); }
    };
    struct RandomDeleter {
        void operator()(mbedtls_ctr_drbg_context* x) { ::mbedtls_ctr_drbg_free(x); }
    };
    struct EntropyDeleter {
        void operator()(mbedtls_entropy_context* x) { ::mbedtls_entropy_free(x); }
    };

    using TLS_Net = std::unique_ptr<mbedtls_net_context, NetDeleter>;
    using TLS_SSL = std::unique_ptr<mbedtls_ssl_context, SSLDeleter>;
    using TLS_Config = std::unique_ptr<mbedtls_ssl_config, ConfigDeleter>;
    using TLS_Random = std::unique_ptr<mbedtls_ctr_drbg_context, RandomDeleter>;
    using TLS_Entropy = std::unique_ptr<mbedtls_entropy_context, EntropyDeleter>;

    const network::ConnectionOptions& mrOptions;
    TLS_Net mpNet;
    TLS_Entropy mpEntropy;
    TLS_Random mpRandom;
    TLS_SSL mpSsl;
    TLS_Config mpConfig;
};

} // rsp::security

#endif //RSP_CORE_LIB_INCLUDE_SECURITY_MBEDTLS_TLS_SOCKET_H
