/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Kim Lindberg Schwaner
 */

#include <utils/VersionNumber.h>

#include <doctest.h>

#include <format>
#include <limits>
#include <string_view>
#include <string>

TEST_SUITE_BEGIN("Utils");

TEST_CASE("VersionNumber")
{
    using namespace std::literals;
    using namespace rsp::utils;

    SUBCASE("Getters") {
        static_assert(VersionNumber{}.Major() == 0);
        static_assert(VersionNumber{}.Minor() == 0);
        static_assert(VersionNumber{}.Patch() == 0);
        static_assert(VersionNumber{1, 2, 3}.Major() == 1);
        static_assert(VersionNumber{1, 2, 3}.Minor() == 2);
        static_assert(VersionNumber{1, 2, 3}.Patch() == 3);

        VersionNumber v0;
        CHECK_EQ(v0.Major(), 0);
        CHECK_EQ(v0.Minor(), 0);
        CHECK_EQ(v0.Patch(), 0);

        VersionNumber v1{1, 2, 3};
        CHECK_EQ(v1.Major(), 1);
        CHECK_EQ(v1.Minor(), 2);
        CHECK_EQ(v1.Patch(), 3);

        unsigned umax = std::numeric_limits<unsigned>::max();
        VersionNumber v2{umax, umax, umax};
        CHECK_EQ(v2.Major(), umax);
        CHECK_EQ(v2.Minor(), umax);
        CHECK_EQ(v2.Patch(), umax);
    }

    SUBCASE("Comparison") {
        static_assert(VersionNumber{1, 2, 3} == VersionNumber{1, 2, 3});
        CHECK_EQ(VersionNumber{1, 2, 3}, VersionNumber{1, 2, 3});

        static_assert(VersionNumber{1, 2, 3} != VersionNumber{1, 2, 4});
        CHECK_NE(VersionNumber{1, 2, 3}, VersionNumber{1, 2, 4});

        static_assert(VersionNumber{1, 2, 3} < VersionNumber{1, 2, 4});
        CHECK_LT(VersionNumber{1, 2, 3}, VersionNumber{1, 2, 4});

        static_assert(VersionNumber{1, 2, 3} <= VersionNumber{1, 2, 3});
        CHECK_LE(VersionNumber{1, 2, 3}, VersionNumber{1, 2, 3});

        static_assert(VersionNumber{1, 2, 3} <= VersionNumber{1, 2, 4});
        CHECK_LE(VersionNumber{1, 2, 3}, VersionNumber{1, 2, 4});

        static_assert(VersionNumber{1, 2, 4} > VersionNumber{1, 2, 3});
        CHECK_GT(VersionNumber{1, 2, 4}, VersionNumber{1, 2, 3});

        static_assert(VersionNumber{1, 2, 4} >= VersionNumber{1, 2, 4});
        CHECK_GE(VersionNumber{1, 2, 4}, VersionNumber{1, 2, 4});

        static_assert(VersionNumber{1, 2, 4} >= VersionNumber{1, 2, 3});
        CHECK_GE(VersionNumber{1, 2, 4}, VersionNumber{1, 2, 3});

        static_assert(VersionNumber{2, 2, 1} > VersionNumber{1, 2, 3});
        CHECK_GT(VersionNumber{2, 2, 1}, VersionNumber{1, 2, 3});

        static_assert(VersionNumber{1, 3, 1} > VersionNumber{1, 2, 3});
        CHECK_GT(VersionNumber{1, 3, 1}, VersionNumber{1, 2, 3});
    }

    SUBCASE("ToString") {
        static_assert(VersionNumber{}.ToString() == "0.0.0"sv);
        CHECK_EQ(VersionNumber{}.ToString(), "0.0.0"s);

        static_assert(VersionNumber{1, 2, 3}.ToString() == "1.2.3"s);
        CHECK_EQ(VersionNumber{1, 2, 3}.ToString(), "1.2.3"sv);

        constexpr unsigned umax = std::numeric_limits<unsigned>::max();
        CHECK_EQ(VersionNumber{umax, umax, umax}.ToString(), std::format("{0}.{0}.{0}", umax));
    }

    SUBCASE("Parse") {
        static_assert(VersionNumber::Parse("1.2.3"sv) == VersionNumber{1, 2, 3});
        CHECK_EQ(VersionNumber::Parse("1.2.3"s), VersionNumber{1, 2, 3});

        static_assert(VersionNumber::Parse("1.0.0-0.3.7"s) == VersionNumber{1, 0, 0});
        CHECK_EQ(VersionNumber::Parse("1.0.0-0.3.7"sv), VersionNumber{1, 0, 0});

        static_assert(VersionNumber::Parse("1.2.3-alpha.1") == VersionNumber{1, 2, 3});
        CHECK_EQ(VersionNumber::Parse("1.2.3-alpha.1"), VersionNumber{1, 2, 3});

        static_assert(VersionNumber::Parse("1.2.3+build.321") == VersionNumber{1, 2, 3});
        CHECK_EQ(VersionNumber::Parse("1.2.3+build.321"), VersionNumber{1, 2, 3});

        static_assert(VersionNumber::Parse("1.2.3-rc.1+build.123") == VersionNumber{1, 2, 3});
        CHECK_EQ(VersionNumber::Parse("1.2.3-rc.1+build.123"), VersionNumber{1, 2, 3});

        static_assert(VersionNumber::Parse("546.13.534677") == VersionNumber{546, 13, 534677});
        CHECK_EQ(VersionNumber::Parse("546.13.534677"), VersionNumber{546, 13, 534677});

        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("1"sv)));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("-1"s)));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("0.-1")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("0.0.-1")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("1.2")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("1.2.")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse(".")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse(".2.3")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("abc")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("a.b.c")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("1.x.3")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("-1.0.0")));
        CHECK_THROWS(static_cast<void>(VersionNumber::Parse("+1.0.0")));
    }
}

TEST_SUITE_END();
