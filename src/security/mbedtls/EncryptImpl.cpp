/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <security/Encrypt.h>
#include "common.h"

namespace rsp::security {


struct MbedTLSEncrypt : public MbedTLSCryptBase
{
    using MbedTLSCryptBase::MbedTLSCryptBase;

    void Init(const SecureBuffer& arIvSeed, const SecureBuffer& arSecret) override
    {
        auto *cipher_info= getCipher();
        auto rc= mbedtls_cipher_setup(&mCipherCtx, cipher_info);

        if (arIvSeed.size() < mbedtls_cipher_info_get_iv_size(cipher_info)) {
            THROW_WITH_BACKTRACE1(CryptException, "Encryption init vector is too small.");
        }

        size_t key_bit_len = mbedtls_cipher_info_get_key_bitlen(cipher_info);
        if ((arSecret.size() * 8) < mbedtls_cipher_info_get_iv_size(cipher_info)) {
            THROW_WITH_BACKTRACE1(CryptException, "Encryption key is too small.");
        }

        rc = mbedtls_cipher_setkey(&mCipherCtx,
                                  arSecret.data(),
                                  int(key_bit_len),
                                  MBEDTLS_ENCRYPT);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_cipher_setkey failed", rc);
        }
        rc = mbedtls_cipher_set_iv(&mCipherCtx, arIvSeed.data(), 16);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_cipher_set_iv failed", rc);
        }
        rc = mbedtls_cipher_set_padding_mode(&mCipherCtx, MBEDTLS_PADDING_PKCS7);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_cipher_set_padding_mode failed", rc);
        }
        rc = mbedtls_cipher_reset(&mCipherCtx);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_cipher_reset failed", rc);
        }
    }

    void Update(const uint8_t *apData, std::size_t aSize) override
    {
        mData.grow(aSize);
        size_t out_len = 0;

        int rc = mbedtls_cipher_update(&mCipherCtx, apData, aSize, mData.current(), &out_len);
        if (rc) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_cipher_update failed", rc);
        }
        mData.moveOffset(int(out_len));
    }

    SecureBuffer Finalize() override
    {
        mData.grow(0);
        size_t out_len = 0;

//        MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA
        int rc = mbedtls_cipher_finish(&mCipherCtx, mData.current(), &out_len);
        if (rc && (rc != MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA)) {
            THROW_WITH_BACKTRACE2(CryptException, "mbedtls_cipher_finish failed", rc);
        }

        mData.moveOffset(int(out_len));

        // Set encrypted data size now that we know it
        mData.shrinkToOffset();

        return mData.copyToSecureBuffer();
    }
};

std::unique_ptr<CryptBase> Encrypt::MakePimpl(CipherTypes aCipher)
{
    return std::unique_ptr<CryptBase>(new MbedTLSEncrypt(aCipher));
}

}
