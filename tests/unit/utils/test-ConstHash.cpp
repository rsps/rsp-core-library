/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/ConstTypeInfo.h>
#include "doctest.h"
#include <utils/Crc32.h>
#include <utils/Fnv1a.h>
#include <iostream>

using namespace rsp::utils;

static uint32_t hash_32_fnv1a(const char* data)
{
    return Fnv1a::Hash32(data);
}

static uint32_t hash_32_crc(std::string_view value)
{
    return Crc32::Calc(value.data(), value.size());
}


TEST_CASE("ConstHash")
{

    std::string_view s("🦄🌈");

    MESSAGE("Length of " << s << " is " << s.size());
//    for(std::size_t i = 0 ; i <= s.length() ; ++i) {
//        std::cout << std::hex << static_cast<int>(static_cast<unsigned char>(s[i])) << " ";
//    }

    CHECK_EQ(hash_32_fnv1a("example"), 0x8bf23ea1);
    CHECK_EQ(hash_32_fnv1a("🦄🌈"), 0xaaf5fee7);
    CHECK_EQ(hash_32_fnv1a("fnv"), 0xb2f5cb99);

    CHECK_EQ(fnv1a::Hash32Const("example"), 0x8bf23ea1);
    CHECK_EQ(fnv1a::Hash32Const("🦄🌈"), 0xaaf5fee7);
    CHECK_EQ(fnv1a::Hash32Const("fnv"), 0xb2f5cb99);

    CHECK_EQ(fnv1a::Hash64Const("example"), 0x430b1483c8d66041);
    CHECK_EQ(fnv1a::Hash64Const("🦄🌈"), 0xa243ed17175ca587);
    CHECK_EQ(fnv1a::Hash64Const("fnv"), 0xdcb57e18fedc1a99);

    CHECK_EQ(crc32::HashConst("example"), 0x6eec9b9f);
    CHECK_EQ(crc32::HashConst("🦄🌈"), 0x0c961c68);
    CHECK_EQ(crc32::HashConst("fnv"), 0xf103fca0);

    CHECK_EQ(hash_32_crc("example"), 0x6eec9b9f);
    CHECK_EQ(hash_32_crc("🦄🌈"), 0x0c961c68);
    CHECK_EQ(hash_32_crc("fnv"), 0xf103fca0);

    CHECK_EQ("example"_fnv1a, 0x8bf23ea1);
    CHECK_EQ("example"_crc32, 0x6eec9b9f);

}

