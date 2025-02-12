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
#include <network/HttpText.h>
#include <network/StatusLine.h>
#include <network/parser-helpers.h>

using namespace rsp::network;

TEST_SUITE_BEGIN("Network");

TEST_CASE("HttpText")
{
    const std::string_view cNormalResponse("HTTP/1.0 200 Ok\r\nContent-Length: 21\r\nETag: \"1550381373\"\r\n\r\nThis is the body text");
    const std::string_view cHeadResponse("HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 162\r\nConnection: close\r\nDate: Tue, 11 Feb 2025 07:02:09 GMT\r\nServer: lighttpd/1.4.75\r\n\r\n");

    SUBCASE("ICaseCompare") {
        CHECK_FALSE(equal_ascii_case_insensitive(cNormalResponse, "Content-Length"));
        CHECK_FALSE(equal_ascii_case_insensitive("Content-Length: 162", "content-length"));
        CHECK(equal_ascii_case_insensitive("Content-Length", "content-length"));
    }

    SUBCASE("Constructs") {
        CHECK_NOTHROW(HttpText ht(""));
        CHECK_NOTHROW(
                HttpText ht(cNormalResponse);
                StatusLine sl(ht.Line());
            );
    }

    SUBCASE("Parse Head Response") {
        HttpText ht(cHeadResponse);
        StatusLine sl(ht.Line());

        CHECK_EQ(sl.GetHttpVersion(), "HTTP/1.1");
        CHECK_EQ(sl.GetStatusCode(), 400);
        CHECK_EQ(sl.GetReasonPhrase(), "Bad Request");

        CHECK_EQ(ht.FieldName(), "Content-Type");
        CHECK_EQ(ht.FieldValue(), "text/html");

        CHECK_EQ(ht.FieldName(), "Content-Length");
        CHECK_EQ(ht.FieldValue(), "162");

        CHECK_EQ(ht.FieldName(), "Connection");
        CHECK_EQ(ht.FieldValue(), "close");

        CHECK_EQ(ht.FieldName(), "Date");
        CHECK(ht.FieldValue().ends_with(" GMT"));

        CHECK_EQ(ht.FieldName(), "Server");
        CHECK_EQ(ht.FieldValue(), "lighttpd/1.4.75");

        CHECK(ht.IsNewLine());
    }

    SUBCASE("Parse Normal Response") {
        HttpText ht(cNormalResponse);
        StatusLine sl(ht.Line());

        CHECK_EQ(sl.GetHttpVersion(), "HTTP/1.0");
        CHECK_EQ(sl.GetStatusCode(), 200);
        CHECK_EQ(sl.GetReasonPhrase(), "Ok");

        CHECK_EQ(ht.FieldName(), "Content-Length");
        CHECK_EQ(ht.FieldValue(), "21");

        CHECK_EQ(ht.FieldName(), "ETag");
        CHECK_EQ(ht.FieldValue(), "1550381373");

        CHECK(ht.IsNewLine());
        CHECK_NOTHROW(ht.CRLF());

        auto span= ht.Octets(21);
        CHECK_EQ(std::string_view(reinterpret_cast<const char*>(span.data()), span.size()), "This is the body text");
    }

}

TEST_SUITE_END();
