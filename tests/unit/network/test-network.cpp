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
        
        opt.BaseUrl = "https://google.com";
        opt.RequestType = HttpRequestType::GET;

        request.SetOptions(opt);
        
        std::cout << request.Execute(); 
    }

    CHECK_EQ(true, true);
}
