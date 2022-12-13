/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_SHA_H_
#define INCLUDE_SECURITY_SHA_H_

#include <security/SecureBuffer.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <string_view>

namespace rsp::security {

/**
 * \brief Supported hash algorithms
 */
enum class HashAlgorithms {
    Sha1,  /**< Sha1 */
    Sha256,/**< Sha256 */
    Sha3   /**< Sha3 */
};

/**
 * \brief Interface class for pimpl design pattern.
 */
struct DigestImpl {
    static DigestImpl* Create(const SecureBuffer& arSecret, HashAlgorithms aAlgorithm);
    virtual ~DigestImpl() {};
    virtual void Update(const uint8_t *apBuffer, std::size_t aSize) = 0;
    virtual SecureBuffer Finalize() = 0;
};

/**
 * \brief SHA interface class.
 */
class Sha
{
public:
    /**
     * \brief Constructor for HMAC SHA that takes a seed secret and the algorithm to use.
     * \param arSecret The secret used to sign the HMAC
     * \param aAlgorithm SHA algorithm to use.
     */
    Sha(const SecureBuffer& arSecret, HashAlgorithms aAlgorithm);

    /**
     * \brief Constructor for message digest SHA that takes the algorithm to use
     * \param aAlgorithm SHA algorithm to use
     */
    Sha(HashAlgorithms aAlgorithm);
    ~Sha();

    /**
     * \brief Call Update with the data to calculate the hash on. Can be called multiple times.
     * \param apBuffer Pointer to data
     * \param aSize Size of data
     */
    void Update(const uint8_t *apBuffer, std::size_t aSize) { mPimpl->Update(apBuffer, aSize); }

    /**
     * \brief Get the final result of the hash calculation. The object should not longer be used after this call.
     * \return SecureBuffer with hash result.
     */
    SecureBuffer Get() { return mPimpl->Finalize(); }

protected:
    std::unique_ptr<DigestImpl> mPimpl;
};

} /* namespace rsp::utils */

#endif /* INCLUDE_SECURITY_SHA_H_ */
