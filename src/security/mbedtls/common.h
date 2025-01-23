/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_SRC_SECURITY_MBEDTLS_COMMON_H
#define RSP_CORE_LIB_SRC_SECURITY_MBEDTLS_COMMON_H

#include <memory>
#include <mbedtls/aes.h>
#include <mbedtls/cipher.h>
#include <security/CryptBase.h>
#include <security/SecureBuffer.h>
#include <security/Sha.h>
#include <utils/DataContainer.h>
#include <magic_enum.hpp>

namespace rsp::security {

class BlockBuffer : public SecureBuffer
{
public:

    uint8_t* current()
    {
        return data() + mOffset;
    }

    void grow(size_t aSize)
    {
        /**
         * \see https://mbed-tls.readthedocs.io/en/latest/kb/how-to/encrypt-with-aes-cbc/
         * Quote: "The CBC mode for AES assumes that you provide data in blocks of 16 bytes"
         */
        resize(size() + aSize + 16);
    }

    void moveOffset(int aLength)
    {
        mOffset += size_t(aLength);
    }

    void shrinkToOffset()
    {
        resize(mOffset);
    }

    SecureBuffer copyToSecureBuffer()
    {
        return {data(), size()};
    }

protected:
    size_t mOffset = 0;
};

class MbedTLSCryptBase : public CryptBase
{
public:
    explicit MbedTLSCryptBase(CipherTypes aCipher)
        : CryptBase(aCipher)
    {
        mbedtls_cipher_init(&mCipherCtx);
    }

    ~MbedTLSCryptBase() override
    {
        mbedtls_cipher_free(&mCipherCtx);
    }

protected:
    [[nodiscard]] static constexpr std::size_t getKeySize() { return 16; }
    BlockBuffer mData{};
    mbedtls_cipher_context_t mCipherCtx{};

    const mbedtls_cipher_info_t* getCipher()
    {
        mbedtls_cipher_type_t mbed_cipher_type;
        switch(mCipherType) {
            default:
            case CipherTypes::AES_128_CBC:
                mbed_cipher_type = MBEDTLS_CIPHER_AES_128_CBC;
                break;
            case CipherTypes::AES_128_GCM:
                mbed_cipher_type = MBEDTLS_CIPHER_AES_128_GCM;
                break;

            case CipherTypes::AES_192_CBC:
                mbed_cipher_type = MBEDTLS_CIPHER_AES_192_CBC;
                break;
            case CipherTypes::AES_192_GCM:
                mbed_cipher_type = MBEDTLS_CIPHER_AES_192_GCM;
                break;

            case CipherTypes::AES_256_CBC:
                mbed_cipher_type = MBEDTLS_CIPHER_AES_256_CBC;
                break;
            case CipherTypes::AES_256_GCM:
                mbed_cipher_type = MBEDTLS_CIPHER_AES_256_GCM;
                break;
        }
        auto *result = mbedtls_cipher_info_from_type(mbed_cipher_type);
        if (!result) {
            THROW_WITH_BACKTRACE2(CryptException, "The requested cipher is not supported: ", magic_enum::enum_name(mbed_cipher_type).data());
        }
        return result;
    }
};

} // namespace rsp::security

#endif // RSP_CORE_LIB_SRC_SECURITY_MBEDTLS_COMMON_H
