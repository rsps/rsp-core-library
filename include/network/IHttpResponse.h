#ifndef I_HTTPRESPONSE_H
#define I_HTTPRESPONSE_H

#include <map>
#include <string>
#include <network/IHttpRequest.h>


namespace rsp::network::http {



class IHttpRequest;

class IHttpResponse
{
public:
    virtual int GetStatusCode() const = 0;

    virtual const std::map<std::string, std::string>& GetHeaders() const = 0;
    virtual const IHttpResponse& SetHeaders(std::map<std::string, std::string> & headers) = 0;
    virtual const std::map<std::string, std::string>& AddHeader(std::string key, std::string value) = 0;
    
    virtual const IHttpRequest& GetRequest() const = 0;
    virtual const std::string& GetBody() const = 0;

    virtual ~IHttpResponse() { }
};

inline std::ostream& operator<<(std::ostream &aStream, IHttpResponse & resp) 
{
    for (const auto &[k, v] : resp.GetHeaders() )
        aStream << ("[" + k + ", " + v + "]\n");
    return aStream << "\n" << " StatusCode = " << std::to_string(resp.GetStatusCode()) << "\n"; 
}
}

#endif
