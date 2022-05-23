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

        auto & test = request.Execute();

        
        IHttpResponse & pResp = request.Execute(); 
        auto headers = pResp.GetHeaders();
        int code = pResp.GetStatusCode();
        std::cout << code << std::endl;

        //HttpResponse dcasted = dynamic_cast<HttpResponse>(request.Execute()); //nope
        //HttpResponse scasted = static_cast<HttpResponse>(request.Execute()); //also nope
        //IHttpResponse ireq = request.Execute(); // doesn't work
        //auto test = request.Execute(); //doesn't work
        //HttpResponse dumb = (HttpResponse)request.Execute(); //worth a shot

        //std::cout << request.Execute(); //works due to global << overload
        //CHECK_EQ(200, request.);
    }

}
