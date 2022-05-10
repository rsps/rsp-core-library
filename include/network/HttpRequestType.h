#ifndef HTTPREQUESTTYPE_H
#define HTTPREQUESTTYPE_H

namespace rsp::network::http {  
enum class HttpRequestType { 
    NONE,
    GET,
    POST,
    PUT,
    HEAD,
    DELETE 
};
}

#endif