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
#include <utils/ConstTypeInfo.h>

using namespace rsp::utils;

TEST_SUITE_BEGIN("Utils");

class MyType {};

enum class MyEnum {ONE, TWO};

TEST_CASE("ConstTypeInfo")
{
    SUBCASE("Type To String")
    {
        constexpr std::string_view sv_int = NameOf<int>();
        CHECK_EQ(sv_int, "int");

        constexpr std::string_view sv_mytype = NameOf<MyType>();
        CHECK_EQ(sv_mytype, "MyType");

        constexpr std::string_view sv_myenum = NameOf<MyEnum>();
        CHECK_EQ(sv_myenum, "MyEnum");
    }
}

TEST_SUITE_END();
