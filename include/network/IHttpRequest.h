#ifndef I_HTTPREQUEST_H
#define I_HTTPREQUEST_H

#include <memory>
#include <functional>
#include <network/HttpRequestOptions.h>
#include <network/IHttpResponse.h>

namespace rsp::network::http {

class IHttpResponse;

class IHttpRequest
{
public:
    //virtual const IHttpRequest& SetHeaders(std::map<std::string, std::string> & headers) = 0;
    //virtual const std::map<std::string, std::string>& AddHeader(std::string key, std::string value) = 0;

    virtual HttpRequestOptions& GetOptions() = 0;
    virtual IHttpRequest & SetOptions(const HttpRequestOptions opt) = 0;
    
    virtual IHttpRequest & SetHeaders(std::map<std::string, std::string>  & headers) = 0;
    virtual IHttpRequest & SetBody(std::string const & body) = 0;

    virtual IHttpResponse& Execute() = 0;
    virtual void Execute(std::function<void(IHttpResponse&)> callback) = 0;
    virtual ~IHttpRequest()
    {
    }
};

inline std::ostream& operator<<(std::ostream &aStream, IHttpRequest & resp) 
{
    return aStream << "Request print not defined yet.\n";
}

}

#endif
