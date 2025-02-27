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
#include <utils/HexStream.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include <mbedtls/ssl.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace rsp::security {

class EMbedTLS : public rsp::network::NetworkException
{
public:
    using rsp::network::NetworkException::NetworkException;
};

class EMbedTLSError : public EMbedTLS
{
public:
    using error_type_t = int;

    explicit EMbedTLSError(const char* apMsg, error_type_t aErr)
            : EMbedTLS(std::string(apMsg) + " (-" + utils::ToHex(uint16_t(std::abs(aErr))) + ")"),
              mCode(aErr)
    {
        MBEDTLS_ERR_SSL_WANT_READ;
    }

protected:
    error_type_t mCode = 0;
};

#define CHK_0(x) { auto ret = (x); if (ret != 0) { THROW_WITH_BACKTRACE2(EMbedTLSError, #x, ret); }}


} // namespace rsp::security

#endif //RSP_CORE_LIB_SRC_SECURITY_OPENSSL_EXCEPTIONS_H
