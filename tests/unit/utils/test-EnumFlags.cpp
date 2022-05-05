/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <doctest.h>
#include <utils/EnumFlags.h>

using namespace rsp::utils;

enum class TestFlags : std::uint32_t {
    None = 0,
    F1 = 1u << 0,
    F2 = 1u << 1,
    F3 = 1u << 2,
    F4 = 1u << 3,
    F5 = 1u << 4,
    F6 = 1u << 5,
    F7 = 1u << 6,
    F8 = 1u << 7,
    F9 = 1u << 8,
    F10 = 1u << 9,
    F11 = 1u << 10,
    F12 = 1u << 11,
    F13 = 1u << 12,
    F14 = 1u << 13,
    F15 = 1u << 14,
    F16 = 1u << 15,
    F32 = 1u << 31,
//    F33 = 1u << 32,
};

TEST_CASE("EnumFlags")
{
    EnumFlags<TestFlags> flags;

    CHECK_EQ(sizeof(flags), 4);

    CHECK_EQ(flags, TestFlags::None);
    CHECK_EQ(int(flags), 0);

    flags |= TestFlags::F3;
    CHECK_EQ(flags, TestFlags::F3);

    flags |= TestFlags::F10;
    CHECK_EQ(flags, (TestFlags::F3 | TestFlags::F10));

    flags |= TestFlags::F16;
    CHECK_EQ(flags, (TestFlags::F3 | TestFlags::F10 | TestFlags::F16));
    CHECK_EQ(flags & TestFlags::F10, TestFlags::F10);

    flags &= TestFlags::F3 | TestFlags::F16;
    CHECK_EQ(flags, (TestFlags::F3 | TestFlags::F16));

    EnumFlags<TestFlags> other(1u << 31);
    CHECK_EQ(other, TestFlags::F32);

    flags |= other;
    CHECK_EQ(flags, (TestFlags::F3 | TestFlags::F16 | TestFlags::F32));

    flags &= other;
    CHECK_EQ(flags, TestFlags::F32);

}



