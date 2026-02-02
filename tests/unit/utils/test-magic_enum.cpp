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


TEST_SUITE_BEGIN("Utils");

enum States //NOSONAR
{
    One = 1,
    Two,
    Three
};

TEST_CASE("magic_enum")
{
    SUBCASE("count")
    {
        constexpr auto count = magic_enum::enum_count<States>();

        CHECK_EQ(count, 3);

        constexpr auto value = States::One;
        constexpr auto name = magic_enum::enum_name(value);

        CHECK_EQ(name, "One");
    }
}

TEST_SUITE_END();
