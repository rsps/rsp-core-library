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

enum class CipherTypes {
    AES_128_CBC,
    AES_128_GCM,
    AES_192_CBC,
    AES_192_GCM,
    AES_256_CBC,
    AES_256_GCM
};

class CryptBase
{
public:
    CryptBase(CipherTypes aCipher) : mCipherType(aCipher) {}
    virtual ~CryptBase() {}

    virtual void Init(const SecureBuffer& arIvSeed, const SecureBuffer& arSecret) = 0;
    virtual void Update(const std::uint8_t *apData, std::size_t aSize) = 0;
    virtual SecureBuffer Finalize() = 0;

    static SecureBuffer KeyGen(std::string_view aSeed, std::size_t aDesiredKeyLen = 64);

protected:
    CipherTypes mCipherType;
    std::unique_ptr<CryptBase> pImpl{};
};


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
