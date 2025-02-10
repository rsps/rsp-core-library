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

std::shared_ptr<ITLSSocket> ITLSSocket::Create()
{
    return std::make_shared<TLSSocket>();
}


#define CHK_NULL(x) if ((x)==NULL) THROW_WITH_BACKTRACE1(EOpenSSL, "Allocation error")
#define CHK_SSL(err) if ((err)==-1) THROW_WITH_BACKTRACE1(EOpenSSL, ERR_lib_error_string(ERR_get_error()))

TLSSocket::TLSSocket()
{
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
    mpMethod = TLS_client_method();
    CHK_NULL(mpMethod);
    mpContext = SSL_CTX_new(mpMethod);
    CHK_NULL(mpContext);
}

TLSSocket::~TLSSocket()
{
    close(mFd);
    SSL_free(mpSSL);
    SSL_CTX_free(mpContext);
}

TLSSocket& TLSSocket::SetSocket(posix::Socket& arSocket)
{
    mFd = arSocket.GetFd();
    mpSSL = SSL_new(mpContext);
    CHK_NULL(mpSSL);
    SSL_set_fd (mpSSL, mFd);
    int err = SSL_connect (mpSSL);
    CHK_SSL(err);
    return *this;
}

TLSSocket& TLSSocket::SetRootCACert(std::string_view aCaCert)
{
    return *this;
}

TLSSocket& TLSSocket::Close()
{
    SSL_shutdown (mpSSL);  /* send SSL/TLS close_notify */
    return *this;
}

size_t TLSSocket::Write(std::span<const std::byte> aData)
{
    int err = SSL_write (mpSSL, aData.data(), int(aData.size()));
    CHK_SSL(err);
    return size_t(err);
}

size_t TLSSocket::Read(std::span<std::byte> aData)
{
    int err = SSL_read (mpSSL, aData.data(), int(aData.size()));
    CHK_SSL(err);
    return size_t(err);
}

} // rsp::security