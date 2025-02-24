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
#include <network/IHttpRequest.h>
#include <network/IHttpSession.h>
#include <network/MultipartBoundary.h>
#include "EHttpResponse.h"
#include "SocketConnection.h"

namespace rsp::network::ehttp {

class EHttpResponse;
class EHttpSession;

class EHttpRequest : public rsp::network::IHttpRequest
{
public:
    typedef std::function<void(EHttpResponse&)> ResponseCallback_t;

    EHttpRequest();

    [[nodiscard]] const HttpRequestOptions& GetOptions() const override;
    IHttpRequest& SetOptions(const HttpRequestOptions& arOptions) override;
    IHttpRequest& SetBody(HttpBody_t apBody) override;
    [[nodiscard]] const IStreamDataProvider& GetBody() const override;
    IHttpRequest& AddField(const std::string& arFieldName, const std::string& arValue) override;
    IHttpRequest& AddFile(const std::string& arFieldName, posix::FileIO& arFile) override;
    IHttpResponse& Execute() override;
    [[nodiscard]] uintptr_t GetHandle() const override;

protected:
    HttpRequestOptions mOptions{};
    EHttpResponse mResponse;
    std::array<std::byte, 256> mWorkBuffer{};
    MultipartBoundary mBoundary{};
    bool mMultipartFormType = false;

    friend class EHttpSession;
    ResponseCallback_t mResponseCallback{};
    std::optional<std::reference_wrapper<IHttpSession>> mrSession{};
    std::unique_ptr<SocketConnection> mpConnection{};

    void prepareRequest();
    SocketConnection& getConnection();
    std::string formatHeaders();
    IStreamDataProvider& getRequestBody();
};

} // rsp::network::ehttp

#endif //RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_REQUEST_H
