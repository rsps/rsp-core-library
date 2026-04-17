/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Kim Lindberg Schwaner
 */

#include <utils/SemVer.h>

#include <doctest.h>

#include <array>
#include <format>
#include <limits>
#include <string_view>
#include <string>

using namespace std::literals;

TEST_SUITE_BEGIN("Utils");

TEST_CASE("semver::PreRelease")
{
    using rsp::utils::semver::PreRelease;

    static_assert(PreRelease::Part{0u} < PreRelease::Part{"0"});

    SUBCASE("ToString") {
        CHECK_EQ(PreRelease{}.ToString(), ""s);
        CHECK_EQ(PreRelease{1u, "alpha"}.ToString(), "1.alpha"s);
        std::array<PreRelease::Part, 3> parts{"beta", 2u, "rc"};
        CHECK_EQ(PreRelease{parts}.ToString(), "beta.2.rc"s);
    }

    SUBCASE("Parse") {
        CHECK_EQ(PreRelease::Parse("").ToString(), ""s);
        CHECK_EQ(PreRelease::Parse("1.alpha").ToString(), "1.alpha"s);
        CHECK_EQ(PreRelease::Parse("beta.2.rc").ToString(), "beta.2.rc"s);

        CHECK_THROWS(static_cast<void>(PreRelease::Parse(".")));
        CHECK_THROWS(static_cast<void>(PreRelease::Parse("..")));
        CHECK_THROWS(static_cast<void>(PreRelease::Parse("alpha..1")));
        CHECK_THROWS(static_cast<void>(PreRelease::Parse("alpha.1.")));
        CHECK_THROWS(static_cast<void>(PreRelease::Parse(".alpha.1")));
    }
}

TEST_CASE("semver::Version")
{
    using rsp::utils::semver::Version;

    SUBCASE("Getters") {
        static_assert(Version{}.GetMajor() == 0);
        static_assert(Version{}.GetMinor() == 0);
        static_assert(Version{}.GetPatch() == 0);
        static_assert(Version{1, 2, 3}.GetMajor() == 1);
        static_assert(Version{1, 2, 3}.GetMinor() == 2);
        static_assert(Version{1, 2, 3}.GetPatch() == 3);

        Version v0;
        CHECK_EQ(v0.GetMajor(), 0);
        CHECK_EQ(v0.GetMinor(), 0);
        CHECK_EQ(v0.GetPatch(), 0);

        Version v1{1, 2, 3};
        CHECK_EQ(v1.GetMajor(), 1);
        CHECK_EQ(v1.GetMinor(), 2);
        CHECK_EQ(v1.GetPatch(), 3);

        unsigned umax = std::numeric_limits<unsigned>::max();
        Version v2{umax, umax, umax};
        CHECK_EQ(v2.GetMajor(), umax);
        CHECK_EQ(v2.GetMinor(), umax);
        CHECK_EQ(v2.GetPatch(), umax);
    }

    SUBCASE("Comparison") {
        static_assert(Version{1, 2, 3} == Version{1, 2, 3});
        CHECK_EQ(Version{1, 2, 3}, Version{1, 2, 3});

        static_assert(Version{1, 2, 3} != Version{1, 2, 4});
        CHECK_NE(Version{1, 2, 3}, Version{1, 2, 4});

        static_assert(Version{1, 2, 3} < Version{1, 2, 4});
        CHECK_LT(Version{1, 2, 3}, Version{1, 2, 4});

        static_assert(Version{1, 2, 3} <= Version{1, 2, 3});
        CHECK_LE(Version{1, 2, 3}, Version{1, 2, 3});

        static_assert(Version{1, 2, 3} <= Version{1, 2, 4});
        CHECK_LE(Version{1, 2, 3}, Version{1, 2, 4});

        static_assert(Version{1, 2, 4} > Version{1, 2, 3});
        CHECK_GT(Version{1, 2, 4}, Version{1, 2, 3});

        static_assert(Version{1, 2, 4} >= Version{1, 2, 4});
        CHECK_GE(Version{1, 2, 4}, Version{1, 2, 4});

        static_assert(Version{1, 2, 4} >= Version{1, 2, 3});
        CHECK_GE(Version{1, 2, 4}, Version{1, 2, 3});

        static_assert(Version{2, 2, 1} > Version{1, 2, 3});
        CHECK_GT(Version{2, 2, 1}, Version{1, 2, 3});

        static_assert(Version{1, 3, 1} > Version{1, 2, 3});
        CHECK_GT(Version{1, 3, 1}, Version{1, 2, 3});

        // pre-release has lower precedence than without
        CHECK_LT(Version::Parse("1.0.0-pre"), Version::Parse("1.0.0"));

        // pre-release ordering (from spec example 11.4)
        CHECK_LT(Version::Parse("1.0.0-alpha"), Version::Parse("1.0.0-alpha.1"));
        static_assert(Version::Parse("1.0.0-alpha") < Version::Parse("1.0.0-alpha.1"));
        CHECK_LT(Version::Parse("1.0.0-alpha.1"), Version::Parse("1.0.0-alpha.beta"));
        static_assert(Version::Parse("1.0.0-alpha.1") < Version::Parse("1.0.0-alpha.beta"));
        CHECK_LT(Version::Parse("1.0.0-alpha.beta"), Version::Parse("1.0.0-beta"));
        static_assert(Version::Parse("1.0.0-alpha.beta") < Version::Parse("1.0.0-beta"));
        CHECK_LT(Version::Parse("1.0.0-beta"), Version::Parse("1.0.0-beta.2"));
        static_assert(Version::Parse("1.0.0-beta") < Version::Parse("1.0.0-beta.2"));
        CHECK_LT(Version::Parse("1.0.0-beta.2"), Version::Parse("1.0.0-beta.11"));
        static_assert(Version::Parse("1.0.0-beta.2") < Version::Parse("1.0.0-beta.11"));
        CHECK_LT(Version::Parse("1.0.0-beta.11"), Version::Parse("1.0.0-rc.1"));
        static_assert(Version::Parse("1.0.0-beta.11") < Version::Parse("1.0.0-rc.1"));
        CHECK_LT(Version::Parse("1.0.0-rc.1"), Version::Parse("1.0.0"));
        static_assert(Version::Parse("1.0.0-rc.1") < Version::Parse("1.0.0"));

        // Numeric identifiers have lower precedence than non-numeric
        CHECK_GT(Version::Parse("1.0.0-alpha"), Version::Parse("1.0.0-1"));
        static_assert(Version::Parse("1.0.0-alpha") > Version::Parse("1.0.0-1"));

        // build metadata is ignored for precedence
        CHECK_EQ(Version::Parse("1.0.0+build.1"), Version::Parse("1.0.0+build.2"));
        static_assert(Version::Parse("1.0.0+build.1") == Version::Parse("1.0.0+build.2"));
        CHECK_EQ(Version::Parse("1.0.0-alpha+build.1"), Version::Parse("1.0.0-alpha+build.2"));
        static_assert(Version::Parse("1.0.0-alpha+build.1") == Version::Parse("1.0.0-alpha+build.2"));
    }

    SUBCASE("ToString") {
        static_assert(Version{}.ToString() == "0.0.0"sv);
        CHECK_EQ(Version{}.ToString(), "0.0.0"s);

        static_assert(Version{1, 2, 3}.ToString() == "1.2.3"s);
        CHECK_EQ(Version{1, 2, 3}.ToString(), "1.2.3"sv);

        constexpr unsigned umax = std::numeric_limits<unsigned>::max();
        CHECK_EQ(Version{umax, umax, umax}.ToString(), std::format("{0}.{0}.{0}", umax));

        // Round-trip with pre-release and build metadata
        CHECK_EQ(Version::Parse("1.2.3-alpha.1").ToString(), "1.2.3-alpha.1"s);
        CHECK_EQ(Version::Parse("1.2.3+build.321").ToString(), "1.2.3+build.321"s);
        CHECK_EQ(Version::Parse("1.2.3-rc.1+build.123").ToString(), "1.2.3-rc.1+build.123"s);
        CHECK_EQ(Version::Parse("1.0.0-0.3.7").ToString(), "1.0.0-0.3.7"s);
    }

    SUBCASE("Parse") {
        static_assert(Version::Parse("1.2.3"sv) == Version{1, 2, 3});
        CHECK_EQ(Version::Parse("1.2.3"s), Version{1, 2, 3});

        // Pre-release versions have lower precedence than the normal version
        CHECK_LT(Version::Parse("1.0.0-0.3.7"sv), Version{1, 0, 0});
        CHECK_LT(Version::Parse("1.2.3-alpha.1"), Version{1, 2, 3});

        // Build metadata is ignored for comparison
        CHECK_EQ(Version::Parse("1.2.3+build.321"), Version{1, 2, 3});

        // Pre-release with build metadata
        CHECK_LT(Version::Parse("1.2.3-rc.1+build.123"), Version{1, 2, 3});

        static_assert(Version::Parse("546.13.534677") == Version{546, 13, 534677});
        CHECK_EQ(Version::Parse("546.13.534677"), Version{546, 13, 534677});

        CHECK_THROWS(static_cast<void>(Version::Parse("")));
        CHECK_THROWS(static_cast<void>(Version::Parse("1"sv)));
        CHECK_THROWS(static_cast<void>(Version::Parse("-1"s)));
        CHECK_THROWS(static_cast<void>(Version::Parse("0.-1")));
        CHECK_THROWS(static_cast<void>(Version::Parse("0.0.-1")));
        CHECK_THROWS(static_cast<void>(Version::Parse("1.2")));
        CHECK_THROWS(static_cast<void>(Version::Parse("1.2.")));
        CHECK_THROWS(static_cast<void>(Version::Parse(".")));
        CHECK_THROWS(static_cast<void>(Version::Parse(".2.3")));
        CHECK_THROWS(static_cast<void>(Version::Parse("abc")));
        CHECK_THROWS(static_cast<void>(Version::Parse("a.b.c")));
        CHECK_THROWS(static_cast<void>(Version::Parse("1.x.3")));
        CHECK_THROWS(static_cast<void>(Version::Parse("-1.0.0")));
        CHECK_THROWS(static_cast<void>(Version::Parse("+1.0.0")));
    }
}

TEST_SUITE_END();
