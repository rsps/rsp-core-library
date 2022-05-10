#ifndef CURLHTTPREQUEST_H
#define CURLHTTPREQUEST_H


#include <network/IHttpRequest.h>
#include <network/HttpResponse.h>

namespace rsp::network::http::curl{
    
    class CurlHttpRequest : public IHttpRequest {
public:
    CurlHttpRequest();
    void Execute(std::function<void(rsp::network::http::IHttpResponse&)> callback) override;
    rsp::network::http::IHttpResponse& Execute() override;
    rsp::network::http::HttpRequestOptions& GetOptions() override;
protected:
    HttpResponse resp;
    HttpRequestOptions opt;
};
}

#endif
