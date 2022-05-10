#ifndef I_HTTPRESPONSE_H
#define I_HTTPRESPONSE_H

#include <map>

#include <network/IHttpRequest.h>


namespace rsp::network::http {  

class IHttpRequest;

class IHttpResponse {
    public:
        virtual int GetStatusCode() const;
        virtual const std::map<std::string, std::string>& GetHeaders() const;
        virtual const IHttpRequest& GetRequest() const;
        virtual const std::string& GetBody() const;
        virtual ~IHttpResponse(){};
};
}

#endif