/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <iomanip>
#include <utils/Sha3.h>
#include <utils/CoreException.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>

namespace rsp::utils {

class OpenSSLSha3 : public DigestImpl
{
public:
    OpenSSLSha3(std::string_view aSecret)
        : mpCtx(HMAC_CTX_new())
    {
        HMAC_Init_ex(mpCtx, aSecret.data(), aSecret.size(), EVP_sha3_256(), NULL);
    }

    ~OpenSSLSha3()
    {
        HMAC_CTX_free(mpCtx);
    }

    OpenSSLSha3(const OpenSSLSha3&) = delete;
    OpenSSLSha3& operator=(const OpenSSLSha3&) = delete;

    void Update(const uint8_t *apBuffer, std::size_t aSize) override
    {
        HMAC_Update(mpCtx, apBuffer, aSize);
    }

    MessageDigest Finalize() override
    {
        MessageDigest result;
        unsigned int len;

        HMAC_Final(mpCtx, result.data(), &len);
        ASSERT(len != result.size());

        return result;
    }

protected:
    HMAC_CTX *mpCtx;
};


std::ostream& operator<<(std::ostream& os, const MessageDigest &arMD)
{
    os << std::hex << std::setw(2);
    for (auto c : arMD) {
        os << static_cast<int>(c);
    }
    return os << std::dec;
}


Sha3::Sha3(std::string_view aSecret)
    : mPimpl(new OpenSSLSha3(aSecret))
{
}

Sha3::~Sha3()
{
}


} /* namespace rsp::utils */

