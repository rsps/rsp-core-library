/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <rsp/compression/ZLib.h>

#include <doctest.h>

#include <string>
#include <vector>

using namespace rsp::compression;

TEST_SUITE_BEGIN("Compression");

TEST_CASE("ZLib")
{
    // clang-format off
    // zlib.compress(b"The quick brown fox jumps over the lazy dog. " * 5, 6)
    const std::vector<uint8_t> cCompressed{
        0x78, 0x9c, 0x0b, 0xc9, 0x48, 0x55, 0x28, 0x2c, 0xcd, 0x4c, 0xce, 0x56, 0x48,
        0x2a, 0xca, 0x2f, 0xcf, 0x53, 0x48, 0xcb, 0xaf, 0x50, 0xc8, 0x2a, 0xcd, 0x2d,
        0x28, 0x56, 0xc8, 0x2f, 0x4b, 0x2d, 0x52, 0x28, 0x01, 0x4a, 0xe7, 0x24, 0x56,
        0x55, 0x2a, 0xa4, 0xe4, 0xa7, 0xeb, 0x29, 0x84, 0x0c, 0x41, 0xc5, 0x00, 0xd2,
        0x8c, 0x50, 0xc4
    };
    // clang-format on
    const std::string cExpected = "The quick brown fox jumps over the lazy dog. ";
    const std::string cPlainText = cExpected + cExpected + cExpected + cExpected + cExpected;

    // Detect if ZLib support is compiled in
    bool zlib_available = [] {
        try {
            ZLib probe;
            return true;
        }
        catch (const rsp::exceptions::NotImplementedException&) {
            return false;
        }
    }();

    if (zlib_available) {
        SUBCASE("Inflate decompresses to the original data")
        {
            ZLib zlib;
            CHECK_NOTHROW(zlib.Inflate(cCompressed));

            const auto& result = zlib.GetResult();
            std::string decompressed(result.begin(), result.end());

            CHECK_EQ(decompressed, cPlainText);
        }

        SUBCASE("Inflate works with a buffer smaller than the decompressed data")
        {
            ZLib zlib(8);
            CHECK_NOTHROW(zlib.Inflate(cCompressed));

            const auto& result = zlib.GetResult();
            std::string decompressed(result.begin(), result.end());

            CHECK_EQ(decompressed, cPlainText);
        }

        SUBCASE("Inflate throws on corrupt data")
        {
            const std::vector<uint8_t> cGarbage{0x00, 0x01, 0x02, 0x03};
            ZLib zlib;

            CHECK_THROWS_AS(zlib.Inflate(cGarbage), const ZlibException&);
        }
    }
    else {
        SUBCASE("Without ZLib support, construction throws")
        {
            CHECK_THROWS_AS(ZLib zlib, const rsp::exceptions::NotImplementedException&);
        }
    }
}

TEST_SUITE_END();
