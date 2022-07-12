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

        IHttpResponse *resp;
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

        CHECK_EQ(200, resp->GetStatusCode());
    }

    SUBCASE("Invalid Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";
        opt.CertPath = "";
        opt.KeyPath = "";

        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_THROWS_AS(resp = &request.Execute(), NetworkException);
        CHECK_THROWS_WITH_AS(resp = &request.Execute(), doctest::Contains("curl_easy_perform() failed. (56) Failure when receiving data from the peer"), NetworkException);

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

//        CHECK_EQ(resp->GetHeaders().at("content-type"), "text/html");
//        CHECK_EQ(resp->GetBody().size(), 120);
//        CHECK_EQ(200, resp->GetStatusCode());
    }

    SUBCASE("Validated Client") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300";

        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE("Request:\n" << resp->GetRequest());
        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeaders().at("content-type"), "text/html");
        CHECK_EQ(resp->GetBody().size(), 120);
        CHECK_EQ(200, resp->GetStatusCode());
    }

    SUBCASE("File Download") {
        HttpRequest request;
        opt.BaseUrl = "https://server.localhost:44300/image.png";

        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

//        MESSAGE("Request:\n" << resp->GetRequest());
//        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeaders().at("content-type"), "image/png");
        CHECK_EQ(resp->GetBody().size(), 25138);
        CHECK_EQ(200, resp->GetStatusCode());
    }

    SUBCASE("Http2") {
        CHECK_NOTHROW(HttpSession session1);
        HttpSession session;
        bool resp1 = false;
        bool resp2 = false;

        {
            HttpRequest request0;
            CHECK_THROWS_AS(session <<= request0, EAsyncRequest);
        }

        opt.BaseUrl = "https://server.localhost:44300/";
        opt.Uri = "index.html";
        opt.Verbose = 1;
        opt.RequestType = HttpRequestType::HEAD;

        HttpRequest request1(opt);
        request1.SetResponseHandler([&resp1](IHttpResponse& resp) {
//            MESSAGE("Response 1:\n" << resp);
            CHECK_EQ(resp.GetHeaders().at("content-type"), "text/html");
            CHECK_EQ(resp.GetHeaders().at("content-length"), "120");
            if (resp.GetRequest().GetOptions().RequestType == HttpRequestType::HEAD) {
                CHECK_EQ(resp.GetBody().size(), 0);
            }
            else {
                CHECK_EQ(resp.GetBody().size(), 120);
            }
            CHECK_EQ(200, resp.GetStatusCode());
            resp1 = true;
        });

        opt.Uri = "image.png";
        opt.RequestType = HttpRequestType::GET;
        HttpRequest request2(opt);
        request2.SetResponseHandler([&resp2](IHttpResponse& resp) {
//            MESSAGE("Response 2:\n" << resp);
            CHECK_EQ(resp.GetHeaders().at("content-type"), "image/png");
            CHECK_EQ(resp.GetHeaders().at("content-length"), "25138");
            CHECK_EQ(resp.GetBody().size(), 25138);
            CHECK_EQ(200, resp.GetStatusCode());
            resp2 = true;
        });

//      TODO: Maybe improve usage to something like this:
//        session.SetDefaultOptions(opt);
//        session.Head("index.html", [](IHttpResponse& resp) { ... });
//        session.Get("index.html", [&resp1](IHttpResponse& resp) { ... });
//        session.Get("image.png", [&resp2](IHttpResponse& resp) { ... });

        session <<= request1; // HEAD index.html
        CHECK_NOTHROW(session.Execute());

        opt.Uri = "index.html";
        request1.SetOptions(opt);

        session <<= request1; // GET index.html
        session <<= request2; // GET image.png

        CHECK_NOTHROW(session.Execute());

        CHECK(resp1);
        CHECK(resp2);
    }

    std::system("killall lighttpd");
}
