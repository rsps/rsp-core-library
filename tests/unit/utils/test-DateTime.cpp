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
#include <string>
#include <utils/DateTime.h>

using namespace rsp::utils;

TEST_CASE("DateTime")
{
    const std::string ts1("2022-11-08T00:00:00.000");
    const std::string ts2("2022-11-08T15:43:23.813");

    SUBCASE("Zero") {
        DateTime dt;

        MESSAGE("DateTime() = " << dt);

        CHECK_EQ(dt.ToRFC3339Milli(), "-4713-11-24T00:00:00.000");

        std::tm tm = dt;
        CHECK_EQ(tm.tm_year, -4713 - 1900);
        CHECK_EQ(tm.tm_mon, 11-1);
        CHECK_EQ(tm.tm_mday, 24);
        CHECK_EQ(tm.tm_hour, 0);
        CHECK_EQ(tm.tm_min, 0);
        CHECK_EQ(tm.tm_sec, 0);
    }

    SUBCASE("Constructors") {
        SUBCASE("From values") {
            DateTime dt(2022, 11, 8, 15, 43, 23);
            MESSAGE("DateTime(2022, 11, 08, 15, 43, 23) = " << dt);
//            MESSAGE("NanoSeconds: " << dt.GetTime().ToNanoSeconds());
            CHECK_EQ(dt.ToLogging(), "2022-11-08 15:43:23.000");
        }

        SUBCASE("From RFC3339Milli string") {
            DateTime dt(ts1, DateTime::Formats::RFC3339Milli);
            MESSAGE("DateTime(" << ts1 << ") = " << dt);
            CHECK_EQ(dt.ToLogging(), "2022-11-08 00:00:00.000");
        }

    }
}



