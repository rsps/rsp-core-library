/*
 * test-strutils.cpp
 *
 *  Created on: Feb 23, 2020
 *      Author: Steffen Brummer
 */

#include <doctest.h>
#include <utils/StrUtils.h>

using namespace rsp;

TEST_CASE("Testing StrUtils") {

    CHECK(StrUtils::GetHomeDir() != "");

    CHECK(StrUtils::GetConfigDir() != "");

    CHECK(StrUtils::EndsWith("[2020-02-23 09:33:48] (6) Test of logger", "Test of logger"));

    CHECK(StrUtils::StartsWith("Hello World", "Hello"));

    CHECK(StrUtils::Contains("Hello World", "llo W"));

    std::string s("Hello World");

    StrUtils::ReplaceAll(s, "llo", "ll of a");
    CHECK(s == "Hell of a World");

    std::chrono::system_clock::time_point tp;
    CHECK(StrUtils::TimeStamp(tp) == "1970-01-01 00:00:00.000");

    // The following should fail if time was set since startup
    CHECK(StrUtils::TimeStamp(std::chrono::steady_clock::now()) == StrUtils::TimeStamp(std::chrono::system_clock::now()));

    CHECK(StrUtils::TimeStamp(std::chrono::milliseconds(0)) == "1970-01-01 00:00:00.000");

}

