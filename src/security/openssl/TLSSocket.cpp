/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "TLSSocket.h"
#include "X509Certs.h"
#include <network/UrlParser.h>

namespace rsp::security {

std::shared_ptr<ITLSSocket> ITLSSocket::Create(network::ConnectionOptions& arOptions)
{
    return std::make_shared<TLSSocket>(arOptions);
}

TLSSocket::TLSSocket(network::ConnectionOptions& arOptions)
    : mrOptions(arOptions)
{
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    auto method = TLS_client_method();
    CHK_NULL(method);
    mpContext = TLS_Context(SSL_CTX_new(method));
    CHK_NULL(mpContext);

    if (!mrOptions.CertCaPath.empty()) {
        X509Certs x509(mpContext.get());
        x509.LoadCertificateAuthority(mrOptions.CertCaPath);
        SSL_CTX_set_verify(mpContext.get(), SSL_VERIFY_PEER, nullptr);
    }

    if (!mrOptions.CertPath.empty()) {
        X509Certs x509(mpContext.get());
        x509.LoadClientCertificate(mrOptions.CertPath, mrOptions.KeyPath);
    }

    if (SSL_CTX_set_min_proto_version(mpContext.get(), TLS1_3_VERSION) <= 0) {
        THROW_WITH_BACKTRACE1(EOpenSSL, "Could not configure minimum protocol version");
    }

    SSL_CTX_set_session_cache_mode(mpContext.get(), SSL_SESS_CACHE_OFF);
}

TLSSocket::~TLSSocket()
{
    if (mpSSL) {
        SSL_shutdown(mpSSL.get());
    }
    close(mFd);
}

TLSSocket& TLSSocket::SetSocket(posix::Socket& arSocket)
{
    mFd = arSocket.GetFd();
    mpSSL = TLS_Connection(SSL_new(mpContext.get()));
    CHK_NULL(mpSSL);

    rsp::network::UrlParser up(mrOptions.BaseUrl);

    std::string host(up.GetHost());

    if (1 != SSL_ctrl(mpSSL.get(), SSL_CTRL_SET_TLSEXT_HOSTNAME, TLSEXT_NAMETYPE_host_name, const_cast<char*>(host.c_str()))) {
        THROW_WITH_BACKTRACE1(EOpenSSL, "Could not enable SNI"); // Server Name Identification
    }
    if (1 != SSL_set1_host(mpSSL.get(), host.c_str())) {
        THROW_WITH_BACKTRACE1(EOpenSSL, "Could not enable host name check");
    }

    SSL_set_fd(mpSSL.get(), mFd);
    int err = SSL_connect(mpSSL.get());
    CHK_SSL(err);

    if (!mrOptions.CertCaPath.empty()) {
        auto const verify_result = SSL_get_verify_result(mpSSL.get());
        if (X509_V_OK != verify_result) {
            THROW_WITH_BACKTRACE1(EOpenSSL, "Certificate verification failed");
        }
    }

    return *this;
}

TLSSocket& TLSSocket::Close()
{
    SSL_shutdown(mpSSL.get());  /* send SSL/TLS close_notify */
    return *this;
}

size_t TLSSocket::Write(std::span<const std::byte> aData)
{
    size_t write_bytes = 0;
    int err = SSL_write_ex(mpSSL.get(), aData.data(), aData.size(), &write_bytes);
    if (!err) {
        THROW_WITH_BACKTRACE1(EOpenSSLError, SSL_get_error(mpSSL.get(), err));
    }
    return size_t(write_bytes);
}

size_t TLSSocket::Read(std::span<std::byte> aData)
{
    size_t read_bytes = 0;
    int err = SSL_read_ex(mpSSL.get(), aData.data(), aData.size(), &read_bytes);
    if (!err) {
        auto code = SSL_get_error(mpSSL.get(), err);
        if (code != SSL_ERROR_ZERO_RETURN) {
            THROW_WITH_BACKTRACE1(EOpenSSLError, code);
        }
    }
    return size_t(read_bytes);
}

} // rsp::security
