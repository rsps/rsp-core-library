#include "curl/CurlHttpRequest.h"
#include <network/HttpRequest.h>
#include <iostream>

namespace rsp::network::http {

HttpRequest::HttpRequest()
    : mResponse(*this),
      mPimpl(new curl::CurlHttpRequest())
{

}

IHttpResponse& HttpRequest::Execute()
{
    return mPimpl->Execute();
}


void HttpRequest::Execute(std::function<void(IHttpResponse&)> callback)
{

}

}

