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
        return this->mPimpl->GetOptions();
    }
    IHttpRequest & SetOptions(const HttpRequestOptions opt) override
    {
        mPimpl->SetOptions(opt);
        return *this;
    }
    IHttpRequest & SetHeaders(std::map<std::string, std::string> & headers) override 
    {
        mPimpl->SetHeaders(headers);
        return *this;
    }

    IHttpRequest & SetBody(std::string const & body) override {
        this->mPimpl->SetBody(body);
        return *this;
    }

    IHttpResponse& Execute() override;
    void Execute(std::function<void(IHttpResponse&)> callback) override;

protected:
    HttpResponse mResponse;
    std::unique_ptr<IHttpRequest> mPimpl;
};
}

#endif
