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
#include <sstream>
#include <ctime>
#include <utils/DateTime.h>

using namespace rsp::utils;

//#define DMESG(a) MESSAGE(a)
#define DMESG(a)

TEST_CASE("DateTime")
{
    const std::string cRFC3339Milli("2022-11-08T15:43:23.813Z");
    const std::string cRFC3339("2022-11-08T15:43:23Z");
    const std::string cISO8601("2022-11-08 15:43:23+0000");
    const std::string cISO8601UTC("2022-11-08 15:43:23");
    const std::string cLogging("2022-11-08 15:43:23.813");
    const std::string cHTTP("Tue, 08 Nov 2022 15:43:23");
    const std::string cCustom("08-2022-11 23:43(15).813"); // Format: "%d-%Y-%m %S:%M:%H."

    SUBCASE("Zero") {
        DateTime dt(0);
        DMESG("DateTime(0) = " << dt)
        CHECK_EQ(dt.ToRFC3339Milli(), "1970-01-01T00:00:00.000Z");

        std::tm tm = dt;
        CHECK_EQ(tm.tm_year, 1970 - 1900);
        CHECK_EQ(tm.tm_mon, 1-1);
        CHECK_EQ(tm.tm_mday, 1);
        CHECK_EQ(tm.tm_hour, 0);
        CHECK_EQ(tm.tm_min, 0);
        CHECK_EQ(tm.tm_sec, 0);
    }

    SUBCASE("Constructors") {

        SUBCASE("Now") {
            DateTime dt1;
            DateTime dt2(std::chrono::system_clock::now());
            CHECK_EQ(dt1.MilliSecondsBetween(dt2), 0); // Test shows difference is way less than 1 microsecond on workstation
        }
        SUBCASE("From values") {
            DateTime dt(2022, 11, 8, 15, 43, 23, 813);
            DMESG("DateTime(2022, 11, 08, 15, 43, 23, 813) = " << dt)
            CHECK_EQ(dt.ToLogging(), cLogging);
        }

        SUBCASE("From RFC3339Milli string") {
            DateTime dt(cRFC3339Milli, DateTime::Formats::RFC3339Milli);
            DMESG("DateTime(" << cRFC3339Milli << ") = " << dt)
            CHECK_EQ(dt.ToLogging(), cLogging);
            CHECK_EQ(dt.ToRFC3339Milli(), cRFC3339Milli);
        }

        SUBCASE("From RFC3339 string") {
            DateTime dt(cRFC3339Milli, DateTime::Formats::RFC3339);
            DMESG("DateTime(" << cRFC3339Milli << ") = " << dt)
            CHECK_EQ(dt.ToRFC3339(), cRFC3339);
        }

        SUBCASE("From ISO8601 string") {
            DateTime dt(cISO8601, DateTime::Formats::ISO8601);
            DMESG("DateTime(" << cISO8601 << ") = " << dt)
            CHECK_EQ(dt.ToISO8601(), cISO8601);
        }

        SUBCASE("From ISO8601UTC string") {
            DateTime dt(cISO8601UTC, DateTime::Formats::ISO8601UTC);
            DMESG("DateTime(" << cISO8601UTC << ") = " << dt)
            CHECK_EQ(dt.ToISO8601UTC(), cISO8601UTC);
        }

        SUBCASE("From Logging string") {
            DateTime dt(cLogging, DateTime::Formats::Logging);
            DMESG("DateTime(" << cLogging << ") = " << dt)
            CHECK_EQ(dt.ToLogging(), cLogging);
        }

        SUBCASE("From HTTP string") {
            DateTime dt(cHTTP, DateTime::Formats::HTTP);
            DMESG("DateTime(" << cHTTP << ") = " << dt)
            CHECK_EQ(dt.ToHTTP(), cHTTP);
        }

        SUBCASE("From custom format") {
            DateTime dt(cCustom, "%d-%Y-%m %S:%M(%H).");
            DMESG("DateTime(" << cCustom << ") = " << dt)
            CHECK_EQ(dt.ToRFC3339Milli(), cRFC3339Milli);
        }

        SUBCASE("From QtGlucobeam Common_Id") {
            const char* common_id = "20230130T104540.925Z200037";
            DateTime dt(common_id, "%Y%m%dT%H%M%S.");
            DateTime same(2023,01,30,10,45,40,925);
            DMESG("DateTime(2023,01,30,10,45,40,925) = " << dt)
            CHECK_EQ(dt, same);
        }

        SUBCASE("From duration") {
            auto t = std::time(nullptr);
            DateTime dt(std::chrono::milliseconds((t * 1000) + 813));
            DMESG("DateTime(" << t << ".813) = " << dt)
            std::time_t result = dt;
            CHECK_EQ(result, t);
        }

        SUBCASE("From time_point") {
            auto tp = std::chrono::system_clock::now();
            DateTime dt(tp);
            DMESG("DateTime(" << tp.time_since_epoch().count() << ") = " << dt)
            std::chrono::system_clock::time_point result = dt;
            CHECK_EQ(result, tp);
        }

        SUBCASE("From file_time") {
            auto tp = std::chrono::file_clock::now();
            DateTime dt(tp);
            DMESG("DateTime(" << tp.time_since_epoch().count() << ") = " << dt)
            std::filesystem::file_time_type result = dt;
            CHECK_EQ(result, tp);
        }

        SUBCASE("From time_t") {
            auto t = std::time(nullptr);
            DateTime dt(t);
            DMESG("DateTime(" << t << ") = " << dt)
            std::time_t result = dt;
            CHECK_EQ(result, t);
        }

        SUBCASE("From tm") {
            std::tm tm{};
            tm.tm_year = 2022 - 1900;
            tm.tm_mon = 11 - 1;
            tm.tm_mday = 8;
            tm.tm_hour = 15;
            tm.tm_min = 43;
            tm.tm_sec = 23;
            tm.tm_isdst = 0;

            DateTime dt(tm);
            std::stringstream ss;
            ss << std::asctime(&tm);
            DMESG("DateTime(" << ss.str() << ") = " << dt)
            DMESG("DateTime(" << cISO8601UTC << ") = " << dt.ToISO8601UTC())
            CHECK_EQ(dt.ToISO8601UTC(), cISO8601UTC);
            std::tm result = dt;
            CHECK_EQ(result.tm_year, tm.tm_year);
            CHECK_EQ(result.tm_mon, tm.tm_mon);
            CHECK_EQ(result.tm_mday, tm.tm_mday);
            CHECK_EQ(result.tm_hour, tm.tm_hour);
            CHECK_EQ(result.tm_min, tm.tm_min);
            CHECK_EQ(result.tm_sec, tm.tm_sec);
            CHECK_EQ(result.tm_isdst, tm.tm_isdst);
        }
    }

    SUBCASE("Between") {
        DateTime dt(cRFC3339Milli, DateTime::Formats::RFC3339Milli);
        std::chrono::system_clock::time_point sc = dt;
        auto offset = std::chrono::nanoseconds(42666444023L); // 42 seconds and a bit...
        DateTime dt2(sc + offset);
        CHECK_EQ(dt.SecondsBetween(dt2), 42);
        CHECK_EQ(dt.MilliSecondsBetween(dt2), 42666);
        CHECK_EQ(dt.MicroSecondsBetween(dt2), 42666444);
        CHECK_EQ(dt.NanoSecondsBetween(dt2), 42666444023L);
    }

    SUBCASE("operators") {
        DateTime dt(cRFC3339Milli, DateTime::Formats::RFC3339Milli);
        auto dt2 = dt;

        SUBCASE("durations") {
            dt2 += std::chrono::milliseconds(12);
            CHECK_EQ(dt.MilliSecondsBetween(dt2), 12);
            CHECK_NE(dt, dt2);

            CHECK_EQ(dt2, (dt + std::chrono::milliseconds(12)));
            CHECK_EQ((dt2 - std::chrono::milliseconds(12)) , dt);

            dt2 -= std::chrono::milliseconds(12);
            CHECK_EQ(dt, dt2);
        }

        SUBCASE("DateTime") {
            DateTime dt3(std::chrono::seconds(13));
            DateTime dt4 = dt + dt3;
            dt2 += dt3;
            CHECK_EQ(dt.SecondsBetween(dt2), 13);
            CHECK_NE(dt, dt2);
            CHECK_EQ(dt2, dt4);
            dt2 -= dt3;
            CHECK_EQ(dt, dt2);
            CHECK_EQ(dt2, (dt4 - dt3));
        }
    }
}



