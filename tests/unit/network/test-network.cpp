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
#include <posix/FileSystem.h>
#include <utils/AnsiEscapeCodes.h>

using namespace rsp::logging;
using namespace rsp::network;
using namespace rsp::utils::AnsiEscapeCodes;

TEST_CASE("Network")
{
    SUBCASE("Library Version"){
        MESSAGE("Network Library: " << NetworkLibrary::Get().GetLibraryName());
        MESSAGE("Network Library Version: " << NetworkLibrary::Get().GetVersion());
    }

    SUBCASE("Online") {
        std::string ip;
        CHECK_NOTHROW(ip = rsp::posix::FileSystem::GetCurrentIpAddress());
        MESSAGE("IP: " << ip);
    }

    SUBCASE("TLS to localhost") {
        HttpRequest request;
        HttpRequestOptions opt;
        opt.BaseUrl = "https://server.localhost:8443";
        opt.CertCaPath = "/tmp/rsp/ca/ca.crt";

        SUBCASE("HEAD") {
            opt.RequestType = HttpRequestType::HEAD;
        }
        SUBCASE("GET") {
            opt.RequestType = HttpRequestType::GET;
        }

        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        MESSAGE("Request:\n" << resp->GetRequest());
        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeaders().at("content-type"), "text/html");

        if (opt.RequestType == HttpRequestType::HEAD) {
            CHECK_EQ(resp->GetBody().size(), 0);
        }
        else {
            CHECK_EQ(resp->GetBody().size(), 58);
        }

        CHECK_EQ(200, resp->GetStatusCode());
    }

    SUBCASE("Validate Client") {
        HttpRequest request;
        HttpRequestOptions opt;
        opt.BaseUrl = "https://server.localhost:44300";
        opt.CertCaPath = "/tmp/rsp/ca/ca.crt";
        opt.CertPath = "/tmp/rsp/certs/SN1234.crt";
        opt.KeyPath = "/tmp/rsp/private/SN1234.key";
        opt.RequestType = HttpRequestType::GET;

        request.SetOptions(opt);

        IHttpResponse *resp;
        CHECK_NOTHROW(resp = &request.Execute());

        MESSAGE("Request:\n" << resp->GetRequest());
        MESSAGE("Response:\n" << *resp);

        CHECK_EQ(resp->GetHeaders().at("content-type"), "text/html");

        CHECK_EQ(resp->GetBody().size(), 58);

        CHECK_EQ(200, resp->GetStatusCode());

    }
}
