#ifndef CONNECTIONOPTIONS_H
#define CONNECTIONOPTIONS_H

#include <string>

namespace rsp::network::http{
    class ConnectionOptions{
        public:
            std::string BaseUrl = "";
            int ConnectionTimeoutMs = 5000;
            int OtherTimeoutMs = 30000;
            std::string certCaPath = "/";
            std::string certKeyPath = "/";
    };
} //namespace rsp::network::http

#endif