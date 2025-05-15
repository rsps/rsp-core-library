/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_REQUEST_H
#define RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_REQUEST_H

#include <filesystem>
#include <memory>
#include <network/HttpRequestBase.h>
#include <network/IHttpSession.h>
#include <network/MultipartBoundary.h>
#include "EHttpResponse.h"
#include "SocketConnection.h"

namespace rsp::network::ehttp {

#ifndef EHTTP_REQUEST_BUFFER_SIZE
    #define EHTTP_REQUEST_BUFFER_SIZE 512
#endif

class EHttpResponse;
class EHttpSession;

class EHttpRequest : public rsp::network::HttpRequestBase
{
public:
    EHttpRequest();

    [[nodiscard]] const HttpRequestOptions& GetOptions() const override;
    IHttpRequest& SetOptions(const HttpRequestOptions& arOptions) override;
    IHttpRequest& SetBody(HttpBody_t apBody) override;
    [[nodiscard]] const IStreamDataProvider& GetBody() const override;
    IHttpResponse& Execute() override;
    [[nodiscard]] uintptr_t GetHandle() const override;

protected:
    using AutoHeaders = std::map<std::string, std::string>;

    HttpRequestOptions mOptions{};
    EHttpResponse mResponse;
    std::array<std::byte, EHTTP_REQUEST_BUFFER_SIZE> mWorkBuffer{};

    friend class EHttpSession;
    std::optional<std::reference_wrapper<IHttpSession>> mrSession{};
    std::unique_ptr<SocketConnection> mpConnection{};

    IHttpResponse& execute() override;

    void prepareRequest(AutoHeaders& arHeaders);
    SocketConnection& getConnection();
    std::string formatHeaders(AutoHeaders& arHeaders);
    IStreamDataProvider& getRequestBody();
};

} // rsp::network::ehttp

#endif //RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_REQUEST_H
