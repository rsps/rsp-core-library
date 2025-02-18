/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_SECURITY_OPENSSL_EXCEPTIONS_H
#define RSP_CORE_LIB_SRC_SECURITY_OPENSSL_EXCEPTIONS_H

#include <network/NetworkException.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

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

    // For ERR_get_error()
    explicit EOpenSSLError(error_type_t aErr)
            : EOpenSSL(std::string(ERR_lib_error_string(aErr)) + " Lib:" + std::to_string(ERR_GET_LIB(aErr)) + ", Reason:" + std::to_string(ERR_GET_REASON(aErr))),
              mCode(aErr)
    {
//    ERR_LIB_SSL = 20;
//    SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED = 1116;
    }

    // For SSL_get_error()
    explicit EOpenSSLError(int aErr)
            : EOpenSSL("SSL error code: " + std::to_string(aErr)),
              mCode(error_type_t(aErr))
    {
        SSL_ERROR_NONE;
    }

    [[nodiscard]] int GetErrorLibrary() const { return ERR_GET_LIB(mCode); }
    [[nodiscard]] int GetErrorReason() const { return ERR_GET_REASON(mCode); }

protected:
    error_type_t mCode = 0;
};

#define CHK_NULL(x) if (!(x)) THROW_WITH_BACKTRACE1(EOpenSSL, "Allocation error")
#define CHK_SSL(err) if ((err) <= 0) THROW_WITH_BACKTRACE1(EOpenSSLError, ERR_get_error())


} // namespace rsp::security

#endif //RSP_CORE_LIB_SRC_SECURITY_OPENSSL_EXCEPTIONS_H
