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

    void Init(std::string_view aIvSeed, std::string_view aSecret) override
    {
        generateKey(mIv, sizeof(mIv), aIvSeed);
        generateKey(mKey, sizeof(mKey), aSecret);

        int rc = EVP_DecryptInit_ex(mCtx.get(), getCipher(), NULL, mKey, mIv);
        if (rc != 1) {
            THROW_WITH_BACKTRACE1(CryptException, "EVP_EncryptInit_ex failed");
        }
    }

    void Update(const uint8_t *apData, std::size_t aSize) override
    {
        mData.grow(aSize);
        int out_len = 0;

        int rc = EVP_DecryptUpdate(mCtx.get(), mData.current(), &out_len, apData, static_cast<int>(aSize));
        if (rc != 1) {
            THROW_WITH_BACKTRACE1(CryptException, "EVP_EncryptUpdate failed");
        }
        mData.moveOffset(out_len);
    }

    SecureBuffer Finalize() override
    {
        mData.grow(0);
        int out_len = 0;

        int rc = EVP_DecryptFinal_ex(mCtx.get(), mData.current(), &out_len);
        if (rc != 1) {
            THROW_WITH_BACKTRACE1(CryptException, "EVP_EncryptFinal_ex failed");
        }

        mData.moveOffset(out_len);

        // Set cipher text size now that we know it
        mData.shrink_to_fit();

        return mData;
    }
};

std::unique_ptr<CryptBase> Decrypt::MakePimpl(CipherTypes aCipher)
{
    return std::unique_ptr<CryptBase>(new OpenSSLDecrypt(aCipher));
}

}

#endif /* USE_OPENSSL */



