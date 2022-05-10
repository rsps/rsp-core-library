#ifndef I_HTTPSESSION_H
#define I_HTTPSESSION_H

#include <network/IHttpRequest.h>
#include <network/HttpRequestOptions.h>

#include <memory>

namespace rsp::network::http {  
class IHttpSession {
    public:
        virtual IHttpRequest& GetDefaultOptions();
        virtual IHttpRequest& MakeRequest();
        virtual IHttpRequest& MakeRequest(HttpRequestOptions options);
        virtual ~IHttpSession(){};
};
}

#endif