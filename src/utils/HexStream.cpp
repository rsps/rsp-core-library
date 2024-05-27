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
#include <utils/StrUtils.h>

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

        os << "0x" << std::setw(int(2 * arHX.mSizeOf)) << std::setfill('0') << std::hex << value;
        os << delim;
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

std::string ToHex(const std::string &arString)
{
    return ToHex(reinterpret_cast<const uint8_t*>(arString.c_str()), arString.size(), 1);
}

std::string ToHex(uint8_t aValue)
{
    std::stringstream ss;
    ss << "0x" << std::setw(2) << std::setfill('0') << std::hex << uint32_t(aValue);
    return ss.str();
}

std::string ToHex(uint16_t aValue)
{
    std::stringstream ss;
    ss << "0x" << std::setw(4) << std::setfill('0') << std::hex << aValue;
    return ss.str();
}

std::string ToHex(uint32_t aValue)
{
    std::stringstream ss;
    ss << "0x" << std::setw(8) << std::setfill('0') << std::hex << aValue;
    return ss.str();
}

std::string ToHex(uint64_t aValue)
{
    std::stringstream ss;
    ss << "0x" << std::setw(16) << std::setfill('0') << std::hex << aValue;
    return ss.str();
}

std::string ToHex(const uint8_t *apData, uint32_t aSize, uint32_t aSizeOf)
{
    std::stringstream out;
    out << rsp::utils::HexStream(apData, aSize, aSizeOf);
    std::string result(out.str());
    StrUtils::Trim(result);
    return result;
}

} /* namespace utils */

