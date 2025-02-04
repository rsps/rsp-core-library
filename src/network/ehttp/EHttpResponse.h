/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_RESPONSE_H
#define RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_RESPONSE_H

#include <map>
#include <network/IHttpResponse.h>
#include <network/IHttpRequest.h>

namespace rsp::network::ehttp {

class EHttpResponse : public IHttpResponse
{
public:
    explicit EHttpResponse(IHttpRequest& arRequest) : mrRequest(arRequest) {}

    [[nodiscard]] const std::map<std::string, std::string>& GetHeaders() const override;
    [[nodiscard]] const std::string& GetHeader(const std::string& arName) const override;
    [[nodiscard]] StatusCodes GetStatusCode() const override;
    [[nodiscard]] const IHttpRequest& GetRequest() const override;
    [[nodiscard]] const std::string& GetBody() const override;

protected:
    IHttpRequest& mrRequest;
    std::map<std::string, std::string> mHeaders{};
    StatusCodes mStatusCode = StatusCodes::Unknown;
    std::string mBody{};
};

} // rsp::network::ehttp

#endif //RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_RESPONSE_H
