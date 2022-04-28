/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <memory>
#include <security/Aes.h>
#include <security/Sha3.h>
#include <utils/CoreException.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace rsp::utils;

namespace rsp::security {

using EvpCipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

Aes::Aes(std::string_view aIvSeed, std::string_view aSecret)
{
    randomize(mIv, cBlockSize, aIvSeed);
    randomize(mKey, cKeySize, aSecret);

    // Load the necessary cipher
    EVP_add_cipher(EVP_aes_256_cbc());
}

Aes::~Aes()
{
    OPENSSL_cleanse(mKey, cKeySize);
    OPENSSL_cleanse(mIv, cBlockSize);
}

void Aes::Encrypt(const std::string &arPlainText, std::string &arCryptText)
{
    EvpCipherCtxPtr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, mKey, mIv);
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptInit_ex failed");

    // Recovered text expands upto BLOCK_SIZE
    arCryptText.resize(arPlainText.size() + cBlockSize);
    int out_len1 = static_cast<int>(arCryptText.size());

    rc = EVP_EncryptUpdate(ctx.get(), static_cast<uint8_t*>(arCryptText.data()), &out_len1, static_cast<const uint8_t*>(arPlainText.data()), static_cast<int>(arPlainText.size()));
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptUpdate failed");

    int out_len2 = static_cast<int>(arCryptText.size()) - out_len1;
    rc = EVP_EncryptFinal_ex(ctx.get(), static_cast<uint8_t*>(arCryptText.data()) + out_len1, &out_len2);
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptFinal_ex failed");

    // Set cipher text size now that we know it
    arCryptText.resize(out_len1 + out_len2);
}

void Aes::Decrypt(const std::string &arCryptText, std::string &arPlainText)
{
    EvpCipherCtxPtr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, mKey, mIv);
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptInit_ex failed");

    // Recovered text contracts upto BLOCK_SIZE
    arPlainText.resize(arCryptText.size());
    int out_len1 = static_cast<int>(arPlainText.size());

    rc = EVP_DecryptUpdate(ctx.get(), static_cast<uint8_t*>(arPlainText.data()), &out_len1, static_cast<const uint8_t*>(arCryptText.data()), static_cast<int>(arCryptText.size()));
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptUpdate failed");

    int out_len2 = static_cast<int>(arPlainText.size()) - out_len1;
    rc = EVP_DecryptFinal_ex(ctx.get(), static_cast<uint8_t*>(arPlainText.data()) + out_len1, &out_len2);
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptFinal_ex failed");

    // Set recovered text size now that we know it
    arPlainText.resize(out_len1 + out_len2);
}


void Aes::randomize(std::uint8_t *apBuffer, int aLen, std::string_view aSeed)
{
    Sha3 sha(aSeed);
    sha.Update(static_cast<uint8_t*>(aSeed.data()), aSeed.size());
    auto md = sha.Get();

    int i = 0;
    for(auto b : md) {
        apBuffer[i++] = b;
        if (i == aLen) {
            break;
        }
    }
}

} /* namespace rsp::security */
