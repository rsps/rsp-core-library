#ifndef I_HTTPREQUEST_H
#define I_HTTPREQUEST_H

#include <memory>
#include <functional>
#include <network/HttpRequestOptions.h>
#include <network/IHttpResponse.h>

namespace rsp::network::http {  
class IHttpResponse;

class IHttpRequest {
    public:
        virtual HttpRequestOptions& GetOptions();
        virtual IHttpResponse& Execute();
        virtual void Execute(std::function<void (IHttpResponse&)> callback);
        virtual ~IHttpRequest(){};
};
}

#endif