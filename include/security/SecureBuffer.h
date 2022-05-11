/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_SECUREBUFFER_H_
#define INCLUDE_SECURITY_SECUREBUFFER_H_

#include <vector>
#include <string>
#include <sstream>
#include "SecureAllocator.h"

namespace rsp::security {

class SecureBuffer;

std::ostream& operator<<(std::ostream& os, const SecureBuffer &arBuffer);


class SecureBuffer : public std::vector<std::uint8_t, SecureAllocator<std::uint8_t>>
{
public:
    using std::vector<std::uint8_t, SecureAllocator<std::uint8_t>>::vector;

    SecureBuffer(const char* apData, std::size_t aSize)
    {
        assign(apData, apData + aSize); // pointers are converted to iterators.
    }

    SecureBuffer(const std::uint8_t* apData, std::size_t aSize)
    {
        assign(apData, apData + aSize); // pointers are converted to iterators.
    }

    bool operator==(const std::string& arOther) const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str() == arOther;
    }

    bool operator==(const char* apOther) const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str() == apOther;
    }

    std::string GetHex() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }
};

} // namespace rsp::security

#endif /* INCLUDE_SECURITY_SECUREBUFFER_H_ */
