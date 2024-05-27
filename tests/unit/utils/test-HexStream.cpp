/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "doctest.h"
#include <utils/HexStream.h>

using namespace rsp::utils;

TEST_CASE("Hex")
{
    SUBCASE("Byte")
    {
        std::uint8_t u = 124u;
        CHECK_EQ(ToHex(u), std::string("0x7c"));
    }

    SUBCASE("Word")
    {
        std::uint16_t u = 61767u;
        CHECK_EQ(ToHex(u), "0xf147");
    }

    SUBCASE("Long")
    {
        std::uint32_t u = 2547932587u;
        CHECK_EQ(ToHex(u), "0x97de5dab");
    }

    SUBCASE("Long Long")
    {
        std::uint64_t u = 9347932587862587583u;
        CHECK_EQ(ToHex(u), "0x81ba872943d8f8bf");
    }

}