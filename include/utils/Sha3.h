/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_SHA3_H_
#define INCLUDE_UTILS_SHA3_H_

#include <cstdint>
#include <array>
#include <memory>
#include <string_view>

namespace rsp::utils
{

class MessageDigest : public std::array<uint8_t, 256/8> {};

std::ostream& operator<<(std::ostream& os, const MessageDigest &arMD);


struct DigestImpl {
    virtual ~DigestImpl();
    virtual void Update(const uint8_t *apBuffer, std::size_t aSize) = 0;
    virtual MessageDigest Finalize() = 0;
};

class Sha3
{
public:
    Sha3(std::string_view aSecret);
    ~Sha3();

    void Update(const uint8_t *apBuffer, std::size_t aSize) { mPimpl->Update(apBuffer, aSize); }
    MessageDigest Get() { return mPimpl->Finalize(); }

protected:
    std::unique_ptr<DigestImpl> mPimpl;
};

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_SHA3_H_ */
