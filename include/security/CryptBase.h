/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_SECURITY_CRYPTBASE_H_
#define INCLUDE_SECURITY_CRYPTBASE_H_

#include <string_view>
#include <vector>
#include <utils/CoreException.h>
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
    CryptBase(CipherTypes aCipher) : mCipherType(aCipher) {}
    virtual ~CryptBase() {}

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
    virtual void Update(const std::uint8_t *apData, std::size_t aSize) = 0;

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
    static SecureBuffer KeyGen(std::string_view aSeed, std::size_t aDesiredKeyLen = 64);

protected:
    CipherTypes mCipherType;
    std::unique_ptr<CryptBase> pImpl{};
};


/**
 * \brief Base class for crypto exceptions.
 */
class CryptException: public rsp::utils::CoreException
{
public:
    explicit CryptException(const char *aMsg, const char *aErr)
        : CoreException(std::string(aMsg) + ": " + std::string(aErr))
    {
    }
};

} // namespace rsp::security

#endif /* INCLUDE_SECURITY_CRYPTBASE_H_ */
