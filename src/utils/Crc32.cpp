/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/Crc32.h>

namespace rsp::utils {


std::uint32_t* Crc32::getTable()
{
    static std::uint32_t mTable[256] = { 0 };

    if (mTable[0] == 0) {
        uint32_t polynomial = 0xEDB88320;
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int j = 0; j < 8; j++) {
                if (c & 1) {
                    c = polynomial ^ (c >> 1);
                }
                else {
                    c >>= 1;
                }
            }
            mTable[i] = c;
        }
    }

    return mTable;
}


std::uint32_t Crc32::Calc(const void* aBuf, std::size_t aLen, std::uint32_t aInitial)
{
    std::uint32_t* table = getTable();
    uint32_t c = aInitial ^ 0xFFFFFFFF;
    const uint8_t* u = static_cast<const uint8_t*>(aBuf);

    for (std::size_t i = 0 ; i < aLen ; i++) {
        c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
}


} /* namespace rsp */
