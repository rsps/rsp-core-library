/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_SECURITY_CRYPT_BASE_H
#define RSP_CORE_LIB_SECURITY_CRYPT_BASE_H

#include <exceptions/CoreException.h>
#include <string_view>
#include <vector>
#include "SecureBuffer.h"

namespace rsp::security {

/**
 * \brief Supported AES algorithms
 */
enum class CipherTypes {
    AES_128_CBC,/**< AES_128_CBC */
    AES_128_GCM,/**< AES_128_GCM */
    AES_192_CBC,/**< AES_192_CBC */
    AES_192_GCM,/**< AES_192_GCM */
    AES_256_CBC,/**< AES_256_CBC */
    AES_256_GCM /**< AES_256_GCM */
};

/**
 * \brief Crypto base class and interface type for pimpl design pattern.
 */
class CryptBase
{
public:
    /**
     * \brief Constructor that takes a cipher to use
     * \param aCipher AES cipher to use for encryption.
     */
    explicit CryptBase(CipherTypes aCipher) : mCipherType(aCipher) {}
    virtual ~CryptBase() = default;

    /**
     * \brief Initialize the crypto module.
     * \param arIvSeed AES initialization vector to use
     * \param arSecret AES symmetric key to use
     */
    virtual void Init(const SecureBuffer& arIvSeed, const SecureBuffer& arSecret) = 0;

    /**
     * \brief Update the crypto operation with more data. Can be called many times.
     * \param apData Pointer to data to operate on.
     * \param aSize Size of data
     */
    virtual void Update(const uint8_t *apData, size_t aSize) = 0;

    /**
     * \brief End the current crypto operation.
     * \return SecureBuffer with output from crypto operation
     */
    virtual SecureBuffer Finalize() = 0;

    /**
     * \brief Helper to generate symmetric key from a given string.
     * \param aSeed String with seed for generator
     * \param aDesiredKeyLen Length of key in bytes.
     * \return SecureBuffer with new key
     */
    static SecureBuffer KeyGen(const SecureBuffer& arSeed, size_t aDesiredKeyLen = 64);

protected:
    CipherTypes mCipherType;
    std::unique_ptr<CryptBase> pImpl{};
};


/**
 * \brief Base class for crypto exceptions.
 */
class CryptException: public exceptions::CoreException
{
public:
    explicit CryptException(const char *apMsg)
            : CoreException(std::string(apMsg))
    {
    }
    explicit CryptException(const char *apMsg, const char *apErr)
        : CoreException(std::string(apMsg) + ": " + std::string(apErr))
    {
    }
    explicit CryptException(const char *aMsg, int aErrCode)
            : CoreException(std::string(aMsg) + ": " + std::to_string(aErrCode))
    {
    }
};

} // namespace rsp::security

#endif // RSP_CORE_LIB_SECURITY_CRYPT_BASE_H
