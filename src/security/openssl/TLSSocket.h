/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_SECURITY_TLS_SOCKET_H
#define RSP_CORE_LIB_INCLUDE_SECURITY_TLS_SOCKET_H

#include "exceptions.h"
#include <posix/Socket.h>
#include <security/ITLSSocket.h>
#include <span>
#include <string_view>

#define OPENSSL_NO_DEPRECATED 1
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

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
    struct ContextDeleter {
        void operator()(SSL_CTX* x) { ::SSL_CTX_free(x); }
    };
    struct ConnectionDeleter {
        void operator()(SSL* x) { ::SSL_free(x); }
    };

    using TLS_Context = std::unique_ptr<SSL_CTX, ContextDeleter>;
    using TLS_Connection = std::unique_ptr<SSL, ConnectionDeleter>;

    network::ConnectionOptions mrOptions;
    TLS_Context mpContext{};
    TLS_Connection mpSSL{};
    int mFd = -1;
};

} // rsp::security

#endif //RSP_CORE_LIB_INCLUDE_SECURITY_TLS_SOCKET_H
