#include <doctest.h>

#include <network/IHttpRequest.h>
#include <network/HttpRequest.h>
#include <network/curl/CurlHttpRequest.h>
#include <iostream>
#include <logging/Logger.h>

using namespace rsp::logging;

using namespace rsp::network::http;

TEST_CASE("Network")
{
    SUBCASE("CurlVersion"){

    }
    SUBCASE("Online"){

    }
    SUBCASE("Google"){
        HttpRequest request;
        HttpRequestOptions opt;
        opt.Headers["haps"] = "snaps";
        opt.BaseUrl = "https://google.com";
        opt.RequestType = HttpRequestType::GET;

        request.SetOptions(opt);

        IHttpResponse & pResp = request.Execute(); 
        auto headers = pResp.GetHeaders();
        
        for (const auto& x : headers) {
            std::cout << x.first << ": " << x.second << "\n";
        }
        int code = pResp.GetStatusCode();
        CHECK_EQ(200,code);

    }

}
