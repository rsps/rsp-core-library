#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <network/IHttpRequest.h>
#include <network/HttpResponse.h>

namespace rsp::network::http {

class HttpRequest: public IHttpRequest
{

public:
    HttpRequest();
    HttpRequestOptions& GetOptions() override
    {
        return mRequestOptions;
    }
    IHttpResponse& Execute() override;
    void Execute(std::function<void(IHttpResponse&)> callback) override;

protected:
    HttpRequestOptions mRequestOptions { };
    HttpResponse mResponse;
    std::unique_ptr<IHttpRequest> mPimpl;
};
}

#endif
