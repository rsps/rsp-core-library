#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <network/IHttpResponse.h>
#include <network/IHttpRequest.h>

namespace rsp::network::http {  
class HttpResponse : public IHttpResponse { 
    public:
        HttpResponse(IHttpRequest& arRequest) : mrRequest(arRequest){};
        int GetStatusCode() const override { return mStatusCode; }
        const std::map<std::string, std::string>& GetHeaders() const override { return mHeaders; }
        const IHttpRequest& GetRequest() const override { return mrRequest; }
        const std::string& GetBody() const override { return mBody; }
        ~HttpResponse(){}

    protected:
        IHttpRequest& mrRequest;
        int mStatusCode = 0;
        std::map<std::string, std::string> mHeaders{};
        std::string mBody{};
};
}

#endif