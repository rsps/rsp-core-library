/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <iomanip>
#include <string>
#include <utils/HexStream.h>

namespace rsp::utils {

std::ostream& operator <<(std::ostream &os, const HexStream &arHX)
{
    std::string_view delim = ", ";
    std::string line;
    size_t mod = arHX.mElementsPerLine / arHX.mSizeOf;
    const uint8_t *data = arHX.mpData;

    for (size_t i = 0 ; i < arHX.mDataSize ; i++) {
        if ((i % mod) == 0) {
            os << "    ";
        }
        if (i == (arHX.mDataSize - 1)) {
            delim = "  ";
        }
        if (arHX.mSizeOf == 1) {
            char ch = *reinterpret_cast<const char*>(data);
            if (ch >= '0' && ch <= 'z') {
                line += ch;
            }
            else {
                line += '.';
            }
        }
        uint32_t value = 0;
        for (uint32_t n=0 ; n < arHX.mSizeOf ; ++n) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            value = value + (uint32_t(*data++) << (8*n));
#else
            value = (value << 8) + *data++;
#endif
        }

        os << "0x" << std::setw(int(2 * arHX.mSizeOf)) << std::setfill('0') << std::hex << value << delim;

        if ((i % mod) == (mod - 1)) {
            os << "  " << line << "\n";
            line.clear();
        }
    }
    if ((arHX.mDataSize % mod) != (mod - 1)) {
        os << std::string((arHX.mElementsPerLine - (arHX.mDataSize % arHX.mElementsPerLine)) * 6, ' ') << "  " << line << "\n";
    }
    os << std::dec;

    return os;
}

} /* namespace utils */

