#include <network/IHttpRequest.h>

namespace rsp::network::http::curl{
    
    class CurlHttpRequest : public IHttpRequest {
public:
    void Execute(std::function<void(rsp::network::http::IHttpResponse&)> callback) override;
    rsp::network::http::IHttpResponse& Execute() override;
    rsp::network::http::HttpRequestOptions& GetOptions() override;
};
}