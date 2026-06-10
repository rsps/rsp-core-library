/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <rsp/utils/Crc32.h>

namespace rsp::utils {

#ifdef LITTLE_ENDIAN
    constexpr uint32_t cCRC32_POLY     = 0xEDB88320;
#else
    constexpr uint32_t cCRC32_POLY     = 0x04C11DB7;
#endif

static const uint32_t cCRC32_XOR_MASK = 0xFFFFFFFF;


const uint32_t* Crc32::getTable()
{
    return crc32::detail::crc_table;
}


uint32_t Crc32::Calc(const void* aBuf, size_t aLen, uint32_t aInitial)
{
    const uint32_t* table = getTable();
    uint32_t c = aInitial ^ cCRC32_XOR_MASK;
    const auto* u = static_cast<const uint8_t*>(aBuf);

    for (size_t i = 0 ; i < aLen ; i++) {
        c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ cCRC32_XOR_MASK;
}

Crc32::Crc32(uint32_t aInitial)
{
    mC = aInitial ^ cCRC32_XOR_MASK;
}

uint32_t Crc32::Add(uint8_t aU)
{
    const uint32_t* table = getTable();

    mC = table[(mC ^ aU) & 0xFF] ^ (mC >> 8);

    return mC ^ cCRC32_XOR_MASK;
}

bool Crc32::Verify(uint32_t aResult, bool aThrowOnMismatch) const
{
    if ((mC ^ cCRC32_XOR_MASK) != aResult) {
        if (aThrowOnMismatch) {
            THROW_WITH_BACKTRACE1(ECrcError, "CRC Error");
        }
        return false;
    }
    return true;
}

} /* namespace rsp::utils */
