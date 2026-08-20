/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      steffen
 */

#include <doctest.h>
#include <rsp/utils/EnumReflection.h>
#include <string_view>

using namespace std::string_view_literals;

TEST_SUITE_BEGIN("Utils");

enum class States
{
    One = 1,
    Two,
    Three
};

TEST_CASE("EnumName")
{
    SUBCASE("count")
    {
        constexpr auto count = rsp::utils::EnumCount<States>();

        // This will only compile if 'count' is truly constexpr
        static_assert(count == 3u);

        // Array sized by constexpr - proves it works at compile time
        std::array<int, count> arr{};
        CHECK_EQ(count, 3u);
        CHECK_EQ(arr.size(), 3u);

        constexpr auto value = States::One;
        constexpr auto name = rsp::utils::EnumName(value);

        CHECK_EQ(name, "One");

        enum class Foo  { SPAM, HAM };
        static_assert(rsp::utils::EnumName(Foo::SPAM) == "SPAM"sv);
    }
}

TEST_SUITE_END();
