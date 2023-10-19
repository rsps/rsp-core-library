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

#include <security/Decrypt.h>

#include "common.h"

namespace rsp::security {


struct OpenSSLDecrypt : public OpenSSLCryptBase
{
    using OpenSSLCryptBase::OpenSSLCryptBase;

    void Init(const SecureBuffer& arIvSeed, const SecureBuffer& arSecret) override
    {
        int rc = EVP_DecryptInit_ex(mCtx.get(), getCipher(), nullptr, arSecret.data(), arIvSeed.data());
        if (rc != 1) {
            THROW_WITH_BACKTRACE2(CryptException, "EVP_DecryptInit_ex failed", ERR_error_string(static_cast<unsigned int>(rc), nullptr));
        }
    }

    void Update(const uint8_t *apData, size_t aSize) override
    {
        mData.grow(aSize);
        int out_len = 0;

        int rc = EVP_DecryptUpdate(mCtx.get(), mData.current(), &out_len, apData, static_cast<int>(aSize));
        if (rc != 1) {
            THROW_WITH_BACKTRACE2(CryptException, "EVP_DecryptUpdate failed", ERR_error_string(static_cast<unsigned int>(rc), nullptr));
        }

        mData.moveOffset(out_len);
    }

    SecureBuffer Finalize() override
    {
        mData.grow(0);
        int out_len = 0;

        int rc = EVP_DecryptFinal_ex(mCtx.get(), mData.current(), &out_len);
        if (rc != 1) {
            THROW_WITH_BACKTRACE2(CryptException, "EVP_DecryptFinal_ex failed", ERR_error_string(static_cast<unsigned int>(rc), nullptr));
        }

        mData.moveOffset(out_len);

        // Set decrypted data size now that we know it
        mData.shrinkToOffset();

        return mData.copyToSecureBuffer();
    }
};

std::unique_ptr<CryptBase> Decrypt::MakePimpl(CipherTypes aCipher)
{
    return std::unique_ptr<CryptBase>(new OpenSSLDecrypt(aCipher));
}

}

#endif /* USE_OPENSSL */
