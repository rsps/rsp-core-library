/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef SRC_SECURITY_OPENSSL_COMMON_H_
#define SRC_SECURITY_OPENSSL_COMMON_H_

#ifdef USE_OPENSSL

#include <memory>
#include <openssl/evp.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <security/CryptBase.h>
#include <security/SecureBuffer.h>
#include <security/Sha.h>
#include <utils/DataContainer.h>

namespace rsp::security {

class BlockBuffer : public SecureBuffer
{
public:

    std::uint8_t* current()
    {
        return data() + mOffset;
    }

    void grow(size_t aSize)
    {
        resize(size() + aSize + EVP_MAX_BLOCK_LENGTH);
    }

    void moveOffset(int aLength)
    {
        mOffset += static_cast<size_t>(aLength);
    }

    void shrinkToOffset()
    {
        resize(mOffset);
    }

protected:
    size_t mOffset = 0;
};

class OpenSSLCryptBase : public CryptBase
{
public:
    using EvpCipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

    OpenSSLCryptBase(CipherTypes aCipher)
        : CryptBase(aCipher),
          mCtx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free)
    {
    }

protected:
    constexpr std::size_t getKeySize() const { return EVP_MAX_IV_LENGTH; }
    BlockBuffer mData{};
    EvpCipherCtxPtr mCtx;

    const EVP_CIPHER* getCipher()
    {
        switch(mCipherType) {
            default:
            case CipherTypes::AES_128_CBC:
                return EVP_aes_128_cbc();
            case CipherTypes::AES_128_GCM:
                return EVP_aes_128_gcm();

            case CipherTypes::AES_192_CBC:
                return EVP_aes_192_cbc();
            case CipherTypes::AES_192_GCM:
                return EVP_aes_192_gcm();

            case CipherTypes::AES_256_CBC:
                return EVP_aes_256_cbc();
            case CipherTypes::AES_256_GCM:
                return EVP_aes_256_gcm();
        }
    }
};

} // namespace rsp::security

#endif /* USE_OPENSSL */

#endif /* SRC_SECURITY_OPENSSL_COMMON_H_ */
