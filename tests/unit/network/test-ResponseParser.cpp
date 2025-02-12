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
#include <network/HttpRequest.h>
#include <network/ResponseParser.h>

using namespace rsp::network;

TEST_SUITE_BEGIN("Network");

TEST_CASE("ResponseParser")
{
    HttpRequest req;
    HttpResponse resp(req);
    ResponseParser rp(resp);

    SUBCASE("Parse Bad Head") {
        const std::string_view cHeadResponse("HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 162\r\nConnection: close\r\nDate: Tue, 11 Feb 2025 07:02:09 GMT\r\nServer: lighttpd/1.4.75\r\n\r\n");
        size_t part1_size = cHeadResponse.size() / 2;
        size_t part2_size = cHeadResponse.size() / 2 + (cHeadResponse.size() % 2);

        auto o = req.GetOptions();
        o.RequestType = HttpRequestType::HEAD;
        req.SetOptions(o);

        CHECK_FALSE(rp.ParseNewData({ reinterpret_cast<const std::byte*>(cHeadResponse.data()), part1_size }));
        CHECK(rp.ParseNewData({ reinterpret_cast<const std::byte*>(cHeadResponse.data() + part1_size), part2_size }));
        CHECK_EQ(resp.GetStatusCode(), StatusCodes::BadRequest);
        CHECK_EQ(resp.GetContentLength(), 162);
        CHECK_EQ(resp.GetHeaders().size(), 5);
        CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
    }

    SUBCASE("Parse Normal Response") {
        const std::string_view cNormalResponse("HTTP/1.0 200 Ok\r\nContent-Length: 21\r\n\r\nThis is the body text");

        CHECK_NOTHROW(rp.ParseNewData({ reinterpret_cast<const std::byte*>(cNormalResponse.data()), cNormalResponse.size() }));
        CHECK_EQ(resp.GetStatusCode(), StatusCodes::Ok);
        CHECK_EQ(resp.GetContentLength(), 21);
        CHECK_EQ(resp.GetHeaders().size(), 1);
    }

}

TEST_SUITE_END();

