/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <utils/Base64.h>
#include <cinttypes>

namespace rsp::utils {

constexpr char cPadding = '=';

std::string Base64::Encode(std::span<const std::byte> aData)
{
    std::string result;
    result.reserve(aData.size() * 4 / 3);

    for (size_t i = 0; i < aData.size() ; i += 3) {
        uint32_t work = (uint32_t(aData[i+0]) << 16)
             | (uint32_t(aData[i+1]) << 8)
             | (uint32_t(aData[i+2]));

        for (size_t b = 0 ; b < 4 ; ++b) {
            size_t sextet = (work >> 18) & 0x3F;
            if (sextet < 26) {
                result += char('A' + sextet);
            }
            else if (sextet < 52) {
                result += char('a' + (sextet - 26));
            }
            else if (sextet < 62) {
                result += char('0' + (sextet - 52));
            }
            else if (sextet < 63) {
                result += '+';
            }
            else if (sextet == 63) {
                result += '/';
            }
            work <<= 6;
        }
    }

    auto padding_count = (aData.size() % 3);
    if (padding_count) {
        padding_count = 3 - padding_count;

        while (padding_count > 0) {
            result[result.size() - padding_count] = cPadding;
            padding_count--;
        }
    }

    return result;
}

std::string Base64::Decode(std::string_view aBase64)
{
    std::string result;
    result.reserve(aBase64.size() * 3 / 4);

    uint32_t padding_count = 0;
    size_t b = 0;
    uint32_t work = 0;
    for (auto c : aBase64) {
        work <<= 6;
        if (c >= 'A' && c <= 'Z') {
            work += uint32_t(c - 'A');
        }
        else if (c >= 'a' && c <= 'z') {
            work += uint32_t(c - 'a' + 26);
        }
        else if (c >= '0' && c <= '9') {
            work += uint32_t(c - '0' + 52);
        }
        else if (c == '+') {
            work += 62;
        }
        else if (c == '/') {
            work += 63;
        }
        else {
            padding_count++;
        }
        b++;
        if (b == 4) {
            switch (padding_count) {
                case 0:
                    result += char((work >> 16) & 0xFF);
                    result += char((work >> 8) & 0xFF);
                    result += char(work & 0xFF);
                    break;
                case 1:
                    result += char((work >> 16) & 0xFF);
                    result += char((work >> 8) & 0xFF);
                    break;
                case 2:
                    result += char((work >> 16) & 0xFF);
                    break;
                default:
                    THROW_WITH_BACKTRACE1(EBase64FormatError, "Base64 string has illegal format");
            }
            work = 0;
            b = 0;
        }
    }
    return result;
}

//std::array<char, 64> Base64::mMap64{
//    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
//    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
//    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
//    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
//};

} // rsp::utils