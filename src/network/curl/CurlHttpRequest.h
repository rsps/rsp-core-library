#ifndef CURLHTTPREQUEST_H
#define CURLHTTPREQUEST_H

#include <network/IHttpRequest.h>
#include <network/HttpResponse.h>
#include <network/NetworkException.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <stdexcept>
#include <utility>

namespace rsp::network::http::curl{
    class CurlHttpRequest : public IHttpRequest {
        public:
            CurlHttpRequest();
            void Execute(std::function<void(rsp::network::http::IHttpResponse&)> callback) override;
            IHttpResponse& Execute() override;
            HttpRequestOptions& GetOptions() override;
            IHttpRequest & SetHeaders(std::map<std::string, std::string>  & headers) override;
            IHttpRequest& SetOptions(const HttpRequestOptions opt) override;
            IHttpRequest & SetBody(std::string const & body) override;

        protected:
            HttpResponse mResponse;
            HttpRequestOptions mRequestOptions;
            static size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data);
            static size_t header_callback(void *data, size_t size, size_t nmemb, void *userdata);
        private:
            void checkVersion();
            // trim from start
            static inline std::string &ltrim(std::string &s) {  
                s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                        [](int c) {return !std::isspace(c);}));
                return s;
            }

            // trim from end
            static inline std::string &rtrim(std::string &s) { 
                s.erase(std::find_if(s.rbegin(), s.rend(),
                        [](int c) {return !std::isspace(c);}).base(), s.end());
                return s;
            }

            // trim from both ends
            static inline std::string &trim(std::string &s) {  
                return ltrim(rtrim(s));
            }        
    };
}

#endif
