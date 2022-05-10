#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <network/IHttpRequest.h>
#include <network/HttpResponse.h>

namespace rsp::network::http {  
class HttpRequest : public IHttpRequest { 
    
    public:
        HttpRequest();
        HttpRequestOptions& GetOptions(){ return mRequestOptions; }
        IHttpResponse& Execute();
        void Execute(std::function<void (IHttpResponse&)> callback);

    protected:
        HttpRequestOptions mRequestOptions{};
        HttpResponse mResponse;
        std::unique_ptr<IHttpRequest> mPimpl;
};
}

#endif