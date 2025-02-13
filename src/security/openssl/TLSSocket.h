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

#include <network/NetworkException.h>
#include <posix/Socket.h>
#include <security/ITLSSocket.h>
#include <span>
#include <string_view>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace rsp::security {

class EOpenSSL : public rsp::network::NetworkException
{
public:
    using rsp::network::NetworkException::NetworkException;
};

class EOpenSSLError : public EOpenSSL
{
public:
    using error_type_t = unsigned long;

    explicit EOpenSSLError(error_type_t aErr)
        : EOpenSSL(std::string(ERR_lib_error_string(aErr)) + " Lib:" + std::to_string(ERR_GET_LIB(aErr)) + ", Reason:" + std::to_string(ERR_GET_REASON(aErr))),
          mCode(aErr)
    {
        ERR_LIB_SSL;
        SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED;
    }

    [[nodiscard]] int GetErrorLibrary() const { return ERR_GET_LIB(mCode); }
    [[nodiscard]] int GetErrorReason() const { return ERR_GET_REASON(mCode); }

protected:
    error_type_t mCode = 0;
};


class TLSSocket : public ITLSSocket
{
public:
    TLSSocket(const network::ConnectionOptions& arOptions);
    ~TLSSocket() override;

//    TLSSocket(const TLSSocket&) = delete;
//    TLSSocket(TLSSocket&&) = default;
//    TLSSocket& operator=(const TLSSocket&) = delete;
//    TLSSocket& operator=(TLSSocket&&) = default;

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

    const network::ConnectionOptions& mrOptions;
    TLS_Context mpContext{};
    TLS_Connection mpSSL{};
    int mFd = -1;
};

} // rsp::security

#endif //RSP_CORE_LIB_INCLUDE_SECURITY_TLS_SOCKET_H
