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
#include <logging/Logger.h>
#include <network/IHttpRequest.h>
#include <network/HttpRequest.h>
#include <utils/AnsiEscapeCodes.h>
#include <iostream>
#include <sstream>

using namespace rsp::logging;
using namespace rsp::network::http;
using namespace rsp::utils::AnsiEscapeCodes;

TEST_CASE("Network")
{
    SUBCASE("CurlVersion"){

    }
    SUBCASE("Online"){

    }
    SUBCASE("Github"){
        HttpRequest request;
        HttpRequestOptions opt;
//        opt.Headers["haps"] = "snaps";
        opt.BaseUrl = "https://github.com";
        opt.RequestType = HttpRequestType::GET;

        request.SetOptions(opt);

        IHttpResponse &resp = request.Execute();
        auto headers = resp.GetHeaders();

        std::stringstream ss;
        for (const auto& x : headers) {
            ss << ec::fg::Green << x.first << ": " << ec::fg::LightCyan << x.second << "\n";
        }

        MESSAGE("Headers:\n" << ss.str());

        CHECK_EQ(resp.GetHeaders()["content-type"], "text/html; charset=utf-8");

        CHECK_EQ(200, resp.GetStatusCode());

        MESSAGE("Body size: " << resp.GetBody().size());
    }

}
