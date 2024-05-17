/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_FOUR_CC_H
#define RSP_CORE_LIB_UTILS_FOUR_CC_H

#include <cstdint>
#include <iostream>
#include <endian.h>


namespace rsp::utils {

constexpr uint32_t fourcc(char const p[5])
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (uint32_t(p[3]) << 24) | (uint32_t(p[2]) << 16) | (uint32_t(p[1]) << 8) | uint32_t(p[0]);
#else
    return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | uint32_t(p[3]);
#endif
}


class FourCC
{
public:
    FourCC() noexcept {}
    explicit FourCC(uint32_t aValue)
    {
        mValue = htobe32(aValue);
    }

    explicit FourCC(const char *apValue)
    {
        for (unsigned int i = 0; i < sizeof(mAscii) ; i++) {
            mAscii[i] = apValue[i];
        }
    }

    explicit operator uint32_t() const { return mValue; }

    bool operator==(const FourCC &arOther) const
    {
//        std::cout << "FourCC compare(" << mValue << ", " << arOther.mValue << ")" << std::endl;
        return mValue == arOther.mValue;
    }

protected:
    friend std::ostream& operator<<(std::ostream& os, const FourCC &arChunk);

    union {
        uint32_t mValue = 0;
        char mAscii[4];
    };
};

std::ostream& operator<<(std::ostream& os, const FourCC &arChunk);


} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_FOUR_CC_H
