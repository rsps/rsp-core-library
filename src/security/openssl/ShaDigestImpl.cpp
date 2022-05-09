/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifdef USE_OPENSSL

#include <security/Sha.h>
#include <utils/CoreException.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>

namespace rsp::security {

/**
 * \class OpenSSLSha
 * \brief OpenSSL implementation of the SHA interface.
 *
 */
class OpenSSLSha : public DigestImpl
{
public:
    OpenSSLSha(std::string_view aSecret, HashAlgorithms aAlgorithm)
        : mpCtx(HMAC_CTX_new())
    {
        const EVP_MD *algo;
        switch (aAlgorithm) {
            case HashAlgorithms::Sha1:
                algo = EVP_sha1();
                break;

            default:
            case HashAlgorithms::Sha256:
                algo = EVP_sha256();
                break;

            case HashAlgorithms::Sha3:
                algo = EVP_sha3_256();
                break;
        }
        HMAC_Init_ex(mpCtx, aSecret.data(), aSecret.size(), algo, NULL);
    }

    ~OpenSSLSha()
    {
        HMAC_CTX_free(mpCtx);
    }

    OpenSSLSha(const OpenSSLSha&) = delete;
    OpenSSLSha& operator=(const OpenSSLSha&) = delete;

    void Update(const uint8_t *apBuffer, std::size_t aSize) override
    {
        HMAC_Update(mpCtx, apBuffer, aSize);
    }

    SecureBuffer Finalize() override
    {
        SecureBuffer result;
        unsigned int len;

        result.resize(HMAC_size(mpCtx));
        HMAC_Final(mpCtx, result.data(), &len);
        ASSERT(len == result.size());

        return result;
    }

protected:
    HMAC_CTX *mpCtx;
};

// SHA interface factory for OpenSSL
DigestImpl* DigestImpl::Create(std::string_view aSecret, HashAlgorithms aAlgorithm)
{
    return new OpenSSLSha(aSecret, aAlgorithm);
}



} // namespace rsp::security

#endif /* USE_OPENSSL */
