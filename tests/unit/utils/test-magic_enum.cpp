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
#include <magic_enum/magic_enum.hpp>
#include <string_view>

using namespace std::string_view_literals;

TEST_SUITE_BEGIN("Utils");

enum class States
{
    One = 1,
    Two,
    Three
};

/* Compile with clang:
 *
 * clang++ -std=c++23 \
 *   -I/home/steffen/Projects/tgm/rsp-core-library/build/_deps/doctest-src/doctest \
 *   -I/home/steffen/Projects/tgm/rsp-core-library/build/_deps/magic_enum-src/include \
 *   -I/home/steffen/Projects/tgm/rsp-core-library/include \
 *   -I/home/steffen/Projects/tgm/rsp-core-library/src \
 *   -I/home/steffen/Projects/tgm/rsp-core-library/tests/helpers \
 *   -pedantic -Wall -Wextra \
 *   -o test-magic_enum \
 *   tests/unit/utils/test-magic_enum.cpp \
 *   tests/test-main.cpp tests/helpers/TestHelpers.cpp \
 *   tests/helpers/TestLogger.cpp \
 *   build/librsp-core-lib.a -lstdc++exp
 */

TEST_CASE("magic_enum")
{
    SUBCASE("count")
    {
        constexpr auto count = magic_enum::enum_count<States>();

        // This will only compile if 'count' is truly constexpr
        static_assert(count == 3u);

        // Array sized by constexpr - proves it works at compile time
        std::array<int, count> arr{};
        CHECK_EQ(count, 3u);
        CHECK_EQ(arr.size(), 3u);

        constexpr auto value = States::One;
        constexpr auto name = magic_enum::enum_name(value);

        CHECK_EQ(name, "One");

        enum class Foo  { SPAM, HAM };
        static_assert(magic_enum::enum_name(Foo::SPAM) == "SPAM"sv);
    }
}

TEST_SUITE_END();
