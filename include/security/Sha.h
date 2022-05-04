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

enum class HashAlgorithms {
    Sha1,
    Sha256,
    Sha3
};

struct DigestImpl {
    static DigestImpl* Create(std::string_view aSecret, HashAlgorithms aAlgorithm);
    virtual ~DigestImpl() {};
    virtual void Update(const uint8_t *apBuffer, std::size_t aSize) = 0;
    virtual SecureBuffer Finalize() = 0;
};

class Sha
{
public:
    Sha(std::string_view aSecret, HashAlgorithms aAlgorithm);
    ~Sha();

    void Update(const uint8_t *apBuffer, std::size_t aSize) { mPimpl->Update(apBuffer, aSize); }
    SecureBuffer Get() { return mPimpl->Finalize(); }

protected:
    std::unique_ptr<DigestImpl> mPimpl;
};

} /* namespace rsp::utils */

#endif /* INCLUDE_SECURITY_SHA_H_ */
