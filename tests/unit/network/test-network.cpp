/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Jesper Madsen, Steffen Brummer
 */

#include <doctest.h>
#include <iostream>
#include <sstream>
#include <logging/Logger.h>
#include <network/IHttpRequest.h>
#include <network/HttpRequest.h>
#include <network/NetworkLibrary.h>
#include <network/HttpSession.h>
#include <network/NetworkException.h>
#include <posix/FileSystem.h>
#include <utils/AnsiEscapeCodes.h>
#include <TestHelpers.h>
#include <cstdlib>

using namespace rsp::logging;
using namespace rsp::network;
using namespace rsp::utils::AnsiEscapeCodes;

TEST_CASE("Network")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    HttpRequestOptions opt;
    opt.CertCaPath = "webserver/ssl/ca/ca.crt";
    opt.CertPath = "webserver/ssl/certs/SN1234.crt";
    opt.KeyPath = "webserver/ssl/private/SN1234.key";

    // Run lighttpd directly from build directory, no need to install it.
    std::system("_deps/lighttpd_src-build/build/lighttpd -f webserver/lighttpd.conf -m _deps/lighttpd_src-build/build");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    SUBCASE("Library Version"){
        CHECK_EQ(NetworkLibrary::Get().GetLibraryName(), "libcurl");
        CHECK_GE(NetworkLibrary::Get().GetVersion(), "7.68.0");
//        MESSAGE("Network Library: " << NetworkLibrary::Get().GetLibraryName());
//        MESSAGE("Network Library Version: " << NetworkLibrary::Get().GetVersion());
    }

    SUBCASE("Online") {
        std::string ip;
        CHECK_NOTHROW(ip = rsp::posix::FileSystem::GetCurrentIpAddress());
        MESSAGE("IP: " << ip);
    }

    SUBCASE("TLS to localhost") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:8443";

        SUBCASE("HEAD") {
            opt.RequestType = HttpRequestType::HEAD;
        }
        SUBCASE("GET") {
            opt.RequestType = HttpRequestType::GET;
        }

        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeaders().at("content-type"), "text/html");

        if (opt.RequestType == HttpRequestType::HEAD) {
            CHECK_EQ(resp->GetBody().size(), 0);
        }
        else {
            CHECK_EQ(resp->GetBody().size(), 120);
        }

        CHECK_EQ(resp->GetStatusCode(), 200);
    }

    SUBCASE("Invalid Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";
        opt.CertPath = "";
        opt.KeyPath = "";

        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;
        CHECK_THROWS_AS(resp = &request.Execute(), NetworkException);
        CHECK_THROWS_WITH_AS(resp = &request.Execute(), doctest::Contains("curl_easy_perform() failed. (56) Failure when receiving data from the peer"), NetworkException);
    }

    SUBCASE("Validated Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";

        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeaders().at("content-type"), "text/html");
        CHECK_EQ(resp->GetBody().size(), 120);
        CHECK_EQ(resp->GetStatusCode(), 200);
    }

    SUBCASE("File Download") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300/image.png";

        request.SetOptions(opt);

        IHttpResponse *resp = nullptr;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeaders().at("content-type"), "image/png");
        CHECK_EQ(resp->GetBody().size(), 25138);
        CHECK_EQ(resp->GetStatusCode(), 200);
    }

    SUBCASE("Http Session") {
        CHECK_NOTHROW(HttpSession session1(1));
        bool respHead = false;
        bool resp1 = false;
        bool resp2 = false;
        HttpSession session(5);

        opt.BaseUrl = "https://server.localhost:44300/";
//        opt.Verbose = 1;
        session.SetDefaultOptions(opt);

        session.Head("index.html",
            [&respHead](IHttpResponse& resp) {
//                MESSAGE("Response Head:\n" << resp);
                CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                CHECK_EQ(resp.GetHeaders().at("content-length"), "120");
                CHECK_EQ(resp.GetBody().size(), 0);
                CHECK_EQ(resp.GetStatusCode(), 200);
                respHead = true;
            });
        CHECK_NOTHROW(session.ProcessRequests());

        session.Get("index.html",
            [&resp1](IHttpResponse& resp) {
//                MESSAGE("Response 1:\n" << resp);
                CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
                CHECK_EQ(resp.GetHeaders().at("content-length"), "120");
                CHECK_EQ(resp.GetBody().size(), 120);
                CHECK_EQ(200, resp.GetStatusCode());
                resp1 = true;
            });
        session.Get("image.png",
            [&resp2](IHttpResponse& resp) {
//              MESSAGE("Response 2:\n" << resp);
                CHECK_EQ(resp.GetHeaders().at("content-type"), "image/png");
                CHECK_EQ(resp.GetHeaders().at("content-length"), "25138");
                CHECK_EQ(resp.GetBody().size(), 25138);
                CHECK_EQ(200, resp.GetStatusCode());
                resp2 = true;
            });
        CHECK_NOTHROW(session.ProcessRequests());

        CHECK(respHead);
        CHECK(resp1);
        CHECK(resp2);
    }

    std::system("killall lighttpd");
}
