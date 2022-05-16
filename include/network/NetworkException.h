#ifndef NETWORKEXCEPTION_H
#define NETWORKEXCEPTION_H

#include <utils/CoreException.h>
#include <string>

namespace rsp::network::exceptions
{
    class NetworkException : public rsp::utils::CoreException{
        public:
            explicit NetworkException(const std::string &aMsg) : rsp::utils::CoreException(aMsg) {}
    };
    class ECurlVersion : public NetworkException{
        public:
            explicit ECurlVersion(const std::string &aMsg) : NetworkException(aMsg) {}
    };
}

#endif