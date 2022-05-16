#ifndef HTTPREQUESTOPTIONS_H
#define HTTPREQUESTOPTIONS_H

#include <network/ConnectionOptions.h>
#include <network/HttpRequestType.h>
#include <memory>
#include <string>
#include <map>

namespace rsp::network::http {  
    class HttpRequestOptions : public ConnectionOptions {
        public:
            std::map<std::string, std::string> Headers = {};
            std::string Uri = "";
            std::string Body = "";
            HttpRequestType RequestType = HttpRequestType::NONE;
            std::string BasicAuthUsername = "";
            std::string BasicAuthPassword = "";
    };
}

#endif