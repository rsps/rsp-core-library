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

TEST_CASE("SemVer")
{
    using rsp::utils::Version;

    SUBCASE("Ctors-Getters-ToString") {
        static_assert(Version::PreRelease{""}.ToString() == ""s);
        static_assert(Version::PreRelease{"1.alpha"}.ToString() == "1.alpha"s);
        static_assert(Version::PreRelease{"beta.2.rc"}.ToString() == "beta.2.rc"s);

        static_assert(Version{}.GetMajor() == 0);
        static_assert(Version{}.GetMinor() == 0);
        static_assert(Version{}.GetPatch() == 0);
        static_assert(Version{}.GetPreRelease() == ""s);
        static_assert(Version{}.GetBuild() == ""s);
        static_assert(Version{}.ToString() == "0.0.0"s);

        static_assert(Version{1, 2, 3}.GetMajor() == 1);
        static_assert(Version{1, 2, 3}.GetMinor() == 2);
        static_assert(Version{1, 2, 3}.GetPatch() == 3);
        static_assert(Version{1, 2, 3}.GetPreRelease() == ""s);
        static_assert(Version{1, 2, 3}.GetBuild() == ""s);
        static_assert(Version{1, 2, 3}.ToString() == "1.2.3"s);

        static_assert(Version{1, 2, 3, "alpha.1"}.GetMajor() == 1);
        static_assert(Version{1, 2, 3, "alpha.1"}.GetMinor() == 2);
        static_assert(Version{1, 2, 3, "alpha.1"}.GetPatch() == 3);
        static_assert(Version{1, 2, 3, "alpha.1"}.GetPreRelease() == "alpha.1"s);
        static_assert(Version{1, 2, 3, "alpha.1"}.GetBuild() == ""s);
        static_assert(Version{1, 2, 3, "alpha.1"}.ToString() == "1.2.3-alpha.1"s);

        static_assert(Version{1, 2, 3, "alpha.1", "build.123"}.GetMajor() == 1);
        static_assert(Version{1, 2, 3, "alpha.1", "build.123"}.GetMinor() == 2);
        static_assert(Version{1, 2, 3, "alpha.1", "build.123"}.GetPatch() == 3);
        static_assert(Version{1, 2, 3, "alpha.1", "build.123"}.GetPreRelease() == "alpha.1"s);
        static_assert(Version{1, 2, 3, "alpha.1", "build.123"}.GetBuild() == "build.123"s);
        static_assert(Version{1, 2, 3, "alpha.1", "build.123"}.ToString() == "1.2.3-alpha.1+build.123"s);

        static_assert(Version{"0.0.0"}.ToString() == "0.0.0"s);
        static_assert(Version{"1.2.3-alpha.1"}.ToString() == "1.2.3-alpha.1"s);
        static_assert(Version{"1.0.0-alpha+001"}.ToString() == "1.0.0-alpha+001"s);
        static_assert(Version{"1.2.3+build.321"}.ToString() == "1.2.3+build.321"s);
        static_assert(Version{"1.2.3-rc.1+build.123"}.ToString() == "1.2.3-rc.1+build.123"s);
        static_assert(Version{"1.0.0-0.3.7"}.ToString() == "1.0.0-0.3.7"s);
        static_assert(Version{"1.0.0+20130313144700"}.ToString() == "1.0.0+20130313144700"s);
        static_assert(Version{"1.0.0-beta+exp.sha.5114f85"}.ToString() == "1.0.0-beta+exp.sha.5114f85"s);
        static_assert(Version{"1.0.0+21AF26D3----117B344092BD"}.ToString() == "1.0.0+21AF26D3----117B344092BD"s);

        CHECK_EQ(Version{"1.2.3-1234.abc.5678.xyz.91011+build.321.zyx.765"}.ToString(), "1.2.3-1234.abc.5678.xyz.91011+build.321.zyx.765"s);

        unsigned umax = std::numeric_limits<unsigned>::max();
        CHECK_EQ(Version{umax, umax, umax}.ToString(), std::format("{0}.{0}.{0}", umax));

        CHECK_THROWS(static_cast<void>(Version::PreRelease{"."}));
        CHECK_THROWS(static_cast<void>(Version::PreRelease{".."}));
        CHECK_THROWS(static_cast<void>(Version::PreRelease{"alpha..1"}));
        CHECK_THROWS(static_cast<void>(Version::PreRelease{"alpha.1."}));
        CHECK_THROWS(static_cast<void>(Version::PreRelease{".alpha.1"}));
        CHECK_THROWS(static_cast<void>(Version{""}));
        CHECK_THROWS(static_cast<void>(Version{"1"sv}));
        CHECK_THROWS(static_cast<void>(Version{"-1"s}));
        CHECK_THROWS(static_cast<void>(Version{"0.-1"}));
        CHECK_THROWS(static_cast<void>(Version{"0.0.-1"}));
        CHECK_THROWS(static_cast<void>(Version{"1.2"}));
        CHECK_THROWS(static_cast<void>(Version{"1.2."}));
        CHECK_THROWS(static_cast<void>(Version{"."}));
        CHECK_THROWS(static_cast<void>(Version{".."}));
        CHECK_THROWS(static_cast<void>(Version{".2"}));
        CHECK_THROWS(static_cast<void>(Version{".2.3"}));
        CHECK_THROWS(static_cast<void>(Version{"abc"}));
        CHECK_THROWS(static_cast<void>(Version{"a.b.c"}));
        CHECK_THROWS(static_cast<void>(Version{"1.x.3"}));
        CHECK_THROWS(static_cast<void>(Version{"-1.0.0"}));
        CHECK_THROWS(static_cast<void>(Version{"+1.0.0"}));
        CHECK_THROWS(static_cast<void>(Version{"1.0.0-."}));
        CHECK_THROWS(static_cast<void>(Version{"1.0.0-.."}));
        CHECK_THROWS(static_cast<void>(Version{"1.0.0-.+build"}));
        CHECK_THROWS(static_cast<void>(Version{"1.0.0-..+build"}));
        CHECK_THROWS(static_cast<void>(Version{"1.0.0-alpha..1"}));
        CHECK_THROWS(static_cast<void>(Version{"1.0.0-alpha.1."}));
        CHECK_THROWS(static_cast<void>(Version{"1.0.0-.alpha.1"}));
    }

    SUBCASE("Comparison") {
        static_assert(Version{1, 2, 3} == Version{1, 2, 3});
        static_assert(Version{1, 2, 3} != Version{1, 2, 4});
        static_assert(Version{1, 2, 3} < Version{1, 2, 4});
        static_assert(Version{1, 2, 3} <= Version{1, 2, 3});
        static_assert(Version{1, 2, 3} <= Version{1, 2, 4});
        static_assert(Version{1, 2, 4} > Version{1, 2, 3});
        static_assert(Version{1, 2, 4} >= Version{1, 2, 4});
        static_assert(Version{1, 2, 4} >= Version{1, 2, 3});
        static_assert(Version{2, 2, 1} > Version{1, 2, 3});

        // pre-release has lower precedence than without
        static_assert(Version{"1.0.0-pre"} < Version{"1.0.0"});

        // pre-release ordering (from spec example 11.4)
        static_assert(Version{"1.0.0-alpha"} < Version{"1.0.0-alpha.1"});
        static_assert(Version{"1.0.0-alpha.1"} < Version{"1.0.0-alpha.beta"});
        static_assert(Version{"1.0.0-alpha.beta"} < Version{"1.0.0-beta"});
        static_assert(Version{"1.0.0-beta"} < Version{"1.0.0-beta.2"});
        static_assert(Version{"1.0.0-beta.2"} < Version{"1.0.0-beta.11"});
        static_assert(Version{"1.0.0-beta.11"} < Version{"1.0.0-rc.1"});
        static_assert(Version{"1.0.0-rc.1"} < Version{"1.0.0"});

        //  Non-numeric pre-release identifiers take precedence over numeric ones
        static_assert(Version::PreRelease::Part{0u} < Version::PreRelease::Part{"0"});
        static_assert(Version{"1.0.0-alpha"} > Version{"1.0.0-1"});

        // build metadata is ignored for precedence
        static_assert(Version{"1.0.0+build.1"} == Version{"1.0.0+build.2"});
        static_assert(Version{"1.0.0-alpha+build.1"} == Version{"1.0.0-alpha+build.2"});

        // just to exercise runtime a little
        Version v{1, 2, 3, "alpha.1.xyz", "build.123"};
        CHECK_EQ(v.ToString(), "1.2.3-alpha.1.xyz+build.123");
        CHECK_EQ(v, Version{"1.2.3-alpha.1.xyz+build.123"});
        CHECK_GT(Version{"1.2.3-alpha.1.xyz.abc+build.123.weee"}, v);
        CHECK_LT(v, Version{"1.2.3-alpha.beta"});
    }
}

TEST_SUITE_END();
