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

#include <exceptions/CoreException.h>
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

class EOpenSSL : public rsp::exceptions::CoreException
{
public:
    using rsp::exceptions::CoreException::CoreException;
};


class TLSSocket : public ITLSSocket
{
public:
    TLSSocket();
    ~TLSSocket() override;

    TLSSocket(const TLSSocket&) = delete;
    TLSSocket(TLSSocket&&) = default;
    TLSSocket& operator=(const TLSSocket&) = delete;
    TLSSocket& operator=(TLSSocket&&) = default;

    TLSSocket& SetSocket(posix::Socket &arSocket) override;
    TLSSocket& SetRootCACert(std::string_view aCaCert) override;
    TLSSocket& Close() override;

    size_t Write(std::span<std::byte const> aData) override;
    size_t Read(std::span<std::byte> aData) override;

protected:
    SSL_CTX* mpContext = nullptr;
    SSL*     mpSSL = nullptr;
    const SSL_METHOD *mpMethod = nullptr;
    int mFd = -1;
};

} // rsp::security

#endif //RSP_CORE_LIB_INCLUDE_SECURITY_TLS_SOCKET_H
