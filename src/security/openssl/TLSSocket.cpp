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

namespace rsp::security {

std::shared_ptr<ITLSSocket> ITLSSocket::Create(const network::ConnectionOptions& arOptions)
{
    return std::make_shared<TLSSocket>(arOptions);
}

#define CHK_NULL(x) if (!(x)) THROW_WITH_BACKTRACE1(EOpenSSL, "Allocation error")
#define CHK_SSL(err) if ((err) <= 0) THROW_WITH_BACKTRACE1(EOpenSSLError, ERR_get_error())

TLSSocket::TLSSocket(const network::ConnectionOptions& arOptions)
    : mrOptions(arOptions)
{
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
    auto method = TLS_client_method();
    CHK_NULL(method);
    mpContext = TLS_Context(SSL_CTX_new(method));
    CHK_NULL(mpContext);
}

TLSSocket::~TLSSocket()
{
    close(mFd);
}

TLSSocket& TLSSocket::SetSocket(posix::Socket& arSocket)
{
    mFd = arSocket.GetFd();
    mpSSL = TLS_Connection(SSL_new(mpContext.get()));
    CHK_NULL(mpSSL);

    std::string host(mrOptions.host);

    if (1 != SSL_ctrl(mpSSL.get(), SSL_CTRL_SET_TLSEXT_HOSTNAME, TLSEXT_NAMETYPE_host_name, const_cast<char*>(host.c_str()))) {
        THROW_WITH_BACKTRACE1(EOpenSSL, "Could not enable SNI"); // Server Name Identification
    }
    if (1 != SSL_set1_host(mpSSL.get(), host.c_str())) {
        THROW_WITH_BACKTRACE1(EOpenSSL, "Could not enable host name check");
    }

    if (!mrOptions.CertCaPath.empty()) {
        BIO *bioCert = BIO_new_mem_buf(mrOptions.CertCaPath.data(), -1);
        X509* pCert = PEM_read_bio_X509(bioCert, nullptr, nullptr, nullptr);
        X509_STORE_add_cert(SSL_CTX_get_cert_store(mpContext.get()), pCert);;

        SSL_CTX_set_verify(mpContext.get(), SSL_VERIFY_PEER, nullptr);

        BIO_free_all(bioCert);
        X509_free(pCert);
    }

    if (!mrOptions.CertPath.empty()) {
        SSL_CTX_use_certificate(mpContext.get(), &ca);
        SSL_CTX_use_PrivateKey(mpContext.get(), &key);
    }

    SSL_CTX_set_options(mpContext.get(), SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);  // other options not used atm  SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2
    SSL_CTX_set_session_cache_mode(mpContext.get(), SSL_SESS_CACHE_OFF);

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
    int err = SSL_write(mpSSL.get(), aData.data(), int(aData.size()));
    CHK_SSL(err);
    return size_t(err);
}

size_t TLSSocket::Read(std::span<std::byte> aData)
{
    int err = SSL_read(mpSSL.get(), aData.data(), int(aData.size()));
    CHK_SSL(err);
    return size_t(err);
}

} // rsp::security