#include <curl/curl.h>
#include <network/HttpRequest.h>
#include "curl/CurlHttpRequest.h"
#include <iostream>

namespace rsp::network::http {



HttpRequest::HttpRequest() 
    : mResponse(*this)
    , mPimpl(new curl::CurlHttpRequest())
{
    
}

IHttpResponse& HttpRequest::Execute()  {
    return mResponse;
}

void HttpRequest::Execute(std::function<void (IHttpResponse&)> callback)  {

}


}

