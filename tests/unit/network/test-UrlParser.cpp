/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <doctest.h>
#include <network/UrlParser.h>

using namespace rsp::network;

TEST_SUITE_BEGIN("Network");

TEST_CASE("UrlParser")
{
    SUBCASE("Empty") {
        CHECK_THROWS_AS(UrlParser(""), EInvalidUrl);
    }

    SUBCASE("Simple") {
        UrlParser up("https://google.com");
        CHECK_EQ(up.GetScheme(), UrlParser::Scheme::Https);
        CHECK_EQ(up.GetHost(), "google.com");
        CHECK_EQ(up.GetPort(), 443);
        CHECK_EQ(up.GetPath(), "/");
    }

    SUBCASE("With Port") {
        UrlParser up("http://google.com:8080");
        CHECK_EQ(up.GetScheme(), UrlParser::Scheme::Http);
        CHECK_EQ(up.GetHost(), "google.com");
        CHECK_EQ(up.GetPort(), 8080);
        CHECK_EQ(up.GetPath(), "/");
        CHECK_EQ(up.GetQuery(), "");
        CHECK_EQ(up.GetFragment(), "");
    }

    SUBCASE("With Path") {
        UrlParser up("http://google.com/api/v1");
        CHECK_EQ(up.GetScheme(), UrlParser::Scheme::Http);
        CHECK_EQ(up.GetHost(), "google.com");
        CHECK_EQ(up.GetPort(), 80);
        CHECK_EQ(up.GetPath(), "/api/v1");
        CHECK_EQ(up.GetQuery(), "");
        CHECK_EQ(up.GetFragment(), "");
    }

    SUBCASE("With Argument") {
        UrlParser up("http://google.com:8080?Arg=fisk");
        CHECK_EQ(up.GetScheme(), UrlParser::Scheme::Http);
        CHECK_EQ(up.GetHost(), "google.com");
        CHECK_EQ(up.GetPort(), 8080);
        CHECK_EQ(up.GetPath(), "/");
        CHECK_EQ(up.GetQuery(), "Arg=fisk");
        CHECK_EQ(up.GetFragment(), "");
        CHECK_FALSE(up.RequiresTLS());
    }

    SUBCASE("Full") {
        UrlParser up("https://user1@google.com:8080/api/v1?Token=1234&Arg=fisk#mouth");
        CHECK_EQ(up.GetScheme(), UrlParser::Scheme::Https);
        CHECK_EQ(up.GetHost(), "google.com");
        CHECK_EQ(up.GetPort(), 8080);
        CHECK_EQ(up.GetUserInfo(), "user1");
        CHECK_EQ(up.GetPath(), "/api/v1");
        CHECK_EQ(up.GetQuery(), "Token=1234&Arg=fisk");
        CHECK_EQ(up.GetFragment(), "mouth");
        CHECK(up.RequiresTLS());
//        MESSAGE(UrlParser("http://user@google.com/redirect?url=https://news.com/finance.php#today"));
    }

    SUBCASE("Invalids") {
        CHECK_THROWS_AS(UrlParser("ftp://google.com"), EInvalidUrl);
        CHECK_THROWS_AS(UrlParser("http:/google.com"), EInvalidUrl);
        CHECK_THROWS_AS(UrlParser("http:///user@google.com/"), EInvalidUrl);
        CHECK_THROWS_AS(UrlParser("http://user@google.com:abc/"), EInvalidUrl);
        CHECK_THROWS_AS(UrlParser("http://@google.com/https://new.com"), EInvalidUrl);
        CHECK_THROWS_AS(UrlParser("http://google.com:555226/"), EInvalidUrl);
        CHECK_THROWS_AS(UrlParser("google.com"), EInvalidUrl);
    }
}

TEST_SUITE_END();
