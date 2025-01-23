
/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <security/CryptBase.h>
#include <security/Sha.h>
#include <mbedtls/md.h>
#include <mbedtls/version.h>
#include <magic_enum.hpp>

namespace rsp::security {

class MbedTLSHDigestBase : public DigestImpl
{
public:
    MbedTLSHDigestBase(HashAlgorithms aAlgorithm, int aHMac)
    {
        mbedtls_md_type_t algo;
        switch (aAlgorithm) {
            case HashAlgorithms::Sha1:
                algo = MBEDTLS_MD_SHA1;
                break;

            default:
            case HashAlgorithms::Sha256:
                algo = MBEDTLS_MD_SHA256;
                break;

            case HashAlgorithms::Sha3:
                algo = MBEDTLS_MD_SHA3_256;
                break;
        }
        mbedtls_md_init(&mDigestCtx);
        mpDigestInfo = mbedtls_md_info_from_type(algo);
        if (!mpDigestInfo) {
            THROW_WITH_BACKTRACE2(CryptException, "The requested algorithm is not supported: ", magic_enum::enum_name(aAlgorithm).data());
        }
        auto rc= mbedtls_md_setup(&mDigestCtx, mpDigestInfo, aHMac);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_md_setup failed", rc);
        }
    }

    ~MbedTLSHDigestBase() override
    {
        mbedtls_md_free(&mDigestCtx);
    }

    [[nodiscard]] std::string GetLibraryVersion() const override
    {
        char result[9];
        mbedtls_version_get_string(result);
        return result;
    }

    [[nodiscard]] std::string GetLibraryName() const override
    {
        return "MbedTLS";
    }

    MbedTLSHDigestBase(const MbedTLSHDigestBase&) = delete;
    MbedTLSHDigestBase& operator=(const MbedTLSHDigestBase&) = delete;

protected:
    mbedtls_md_context_t mDigestCtx{};
    const mbedtls_md_info_t *mpDigestInfo = nullptr;
};


class MbedTLSHMac : public MbedTLSHDigestBase
{
public:
    MbedTLSHMac(const SecureBuffer& arSecret, HashAlgorithms aAlgorithm)
        : MbedTLSHDigestBase(aAlgorithm, 1)
    {
        auto rc = mbedtls_md_hmac_starts(&mDigestCtx, arSecret.data(), arSecret.size());
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_md_hmac_starts failed", rc);
        }
    }

    void Update(const uint8_t *apBuffer, std::size_t aSize) override
    {
        auto rc = mbedtls_md_hmac_update(&mDigestCtx, apBuffer, aSize);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_md_hmac_update failed", rc);
        }
    }

    SecureBuffer Finalize() override
    {
        SecureBuffer result;
        result.resize(size_t(mbedtls_md_get_size(mpDigestInfo)));

        auto rc= mbedtls_md_hmac_finish(&mDigestCtx, result.data());
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_md_hmac_finish failed", rc);
        }

        return result;
    }
};

class MbedTLSSha: public MbedTLSHDigestBase
{
public:
    explicit MbedTLSSha(HashAlgorithms aAlgorithm)
            : MbedTLSHDigestBase(aAlgorithm, 0)
    {
        auto rc = mbedtls_md_starts(&mDigestCtx);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_md_starts failed", rc);
        }
    }

    void Update(const uint8_t *apBuffer, std::size_t aSize) override
    {
        auto rc = mbedtls_md_update(&mDigestCtx, apBuffer, aSize);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_md_update failed", rc);
        }
    }

    SecureBuffer Finalize() override
    {
        SecureBuffer result;
        result.resize(size_t(mbedtls_md_get_size(mpDigestInfo)));

        auto rc= mbedtls_md_finish(&mDigestCtx, result.data());
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_md_finish failed", rc);
        }

        return result;
    }
};

// SHA interface factory for OpenSSL
DigestImpl* DigestImpl::Create(const SecureBuffer& arSecret, HashAlgorithms aAlgorithm)
{
    if (!arSecret.empty()) {
        return new MbedTLSHMac(arSecret, aAlgorithm);
    }
    return new MbedTLSSha(aAlgorithm);
}


} // namespace rsp::security
