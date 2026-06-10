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
#include <rsp/network/HttpRequest.h>
#include <rsp/network/ResponseParser.h>

using namespace rsp::network;

TEST_SUITE_BEGIN("Network");

TEST_CASE("ResponseParser")
{
    HttpRequest req;
    HttpResponse resp(req);
    ResponseParser rp(resp);

    SUBCASE("Parse Bad Head") {
        constexpr std::string_view cResponse("HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 162\r\nConnection: close\r\nDate: Tue, 11 Feb 2025 07:02:09 GMT\r\nServer: lighttpd/1.4.75\r\n\r\n");
        size_t part1_size = cResponse.size() / 2;
        size_t part2_size = cResponse.size() / 2 + (cResponse.size() % 2);

        auto o = req.GetOptions();
        o.RequestType = HttpRequestType::HEAD;
        req.SetOptions(o);

        CHECK_FALSE(rp.ParseNewData({ reinterpret_cast<const std::byte*>(cResponse.data()), part1_size }));
        CHECK(rp.ParseNewData({ reinterpret_cast<const std::byte*>(cResponse.data() + part1_size), part2_size }));
        CHECK_EQ(resp.GetStatusCode(), StatusCodes::BadRequest);
        CHECK_EQ(resp.GetContentLength(), 162u);
        CHECK_EQ(resp.GetHeaders().size(), 5u);
        CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
    }

    SUBCASE("Parse Normal Response") {
        const std::string_view cResponse("HTTP/1.0 200 Ok\r\nX-HttpsTime: 123.4567\r\nContent-Length: 21\r\n\r\nThis is the body text");

        CHECK_NOTHROW(rp.ParseNewData({ reinterpret_cast<const std::byte*>(cResponse.data()), cResponse.size() }));
        CHECK_EQ(resp.GetStatusCode(), StatusCodes::Ok);
        CHECK_EQ(resp.GetContentLength(), 21u);
        CHECK_EQ(resp.GetHeaders().size(), 2u);
        CHECK_EQ(resp.GetHeader("X-HTTPSTIME"), "123.4567");
        CHECK_EQ(string_to_floating_point<double>(resp.GetHeader("X-HTTPSTIME")), 123.4567);
    }

    SUBCASE("Parse No-Content Response") {
        const std::string_view cResponse("HTTP/1.1 204 No Content\r\nX-HttpsTime: 123.4567\r\n\r\n");

        CHECK(rp.ParseNewData({ reinterpret_cast<const std::byte*>(cResponse.data()), cResponse.size() }));
        CHECK_EQ(resp.GetStatusCode(), StatusCodes::NoContent);
        CHECK_EQ(resp.GetContentLength(), 0u);
        CHECK_EQ(resp.GetHeaders().size(), 1u);
        CHECK_EQ(resp.GetHeader("X-HTTPSTIME"), "123.4567");
        CHECK_EQ(string_to_floating_point<double>(resp.GetHeader("X-HTTPSTIME")), 123.4567);
    }
}

TEST_SUITE_END();

