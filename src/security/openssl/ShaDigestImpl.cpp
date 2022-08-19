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

#include <openssl/evp.h>
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
    OpenSSLSha(const SecureBuffer& arSecret, HashAlgorithms aAlgorithm)
    //    : mpCtx(HMAC_CTX_new())
        : mpMac(EVP_MAC_fetch(nullptr, "HMAC", nullptr)),
          mpCtx(EVP_MAC_CTX_new(mpMac))
    {
//        const EVP_MD *algo;
        std::string algo;
        switch (aAlgorithm) {
            case HashAlgorithms::Sha1:
//                algo = EVP_sha1();
                algo = "SHA1";
                break;

            default:
            case HashAlgorithms::Sha256:
//                algo = EVP_sha256();
                algo = "SHA256";
                break;

            case HashAlgorithms::Sha3:
//                algo = EVP_sha3_256();
                algo = "SHA3-256";
                break;
        }
        OSSL_PARAM params[2];
        params[0] = OSSL_PARAM_construct_utf8_string("digest", algo.data(), 0);
        params[1] = OSSL_PARAM_construct_end();

//        HMAC_Init_ex(mpCtx, arSecret.data(), static_cast<int>(arSecret.size()), algo, NULL);
        EVP_MAC_init(mpCtx, arSecret.data(), static_cast<unsigned long int>(arSecret.size()), params);
    }

    ~OpenSSLSha()
    {
//        HMAC_CTX_free(mpCtx);
        EVP_MAC_free(mpMac);
        EVP_MAC_CTX_free(mpCtx);
    }

    OpenSSLSha(const OpenSSLSha&) = delete;
    OpenSSLSha& operator=(const OpenSSLSha&) = delete;

    void Update(const uint8_t *apBuffer, std::size_t aSize) override
    {
//        HMAC_Update(mpCtx, apBuffer, aSize);
        EVP_MAC_update(mpCtx, apBuffer, static_cast<unsigned long int>(aSize));
    }

    SecureBuffer Finalize() override
    {
        SecureBuffer result;
        unsigned long int len;

//        result.resize(HMAC_size(mpCtx));
//        HMAC_Final(mpCtx, result.data(), &len);
        EVP_MAC_final(mpCtx, nullptr, &len, 0);
        result.resize(len);
        EVP_MAC_final(mpCtx, result.data(), &len, len);
        ASSERT(len == result.size());

        return result;
    }

protected:
    //    HMAC_CTX *mpCtx;
    EVP_MAC * mpMac;
    EVP_MAC_CTX *mpCtx;
};

// SHA interface factory for OpenSSL
DigestImpl* DigestImpl::Create(const SecureBuffer& arSecret, HashAlgorithms aAlgorithm)
{
    return new OpenSSLSha(arSecret, aAlgorithm);
}



} // namespace rsp::security

#endif /* USE_OPENSSL */
