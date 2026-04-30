/*!
 * \copyright        Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license          Mozilla Public License 2.0
 * \author:          Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RSP_CORE_LIB_NETWORK_HTTP_RESPONSE_H
#define RSP_CORE_LIB_NETWORK_HTTP_RESPONSE_H

#include <network/IHttpRequest.h>
#include <network/IHttpResponse.h>
#include <network/NetworkException.h>
#include <network/StatusLine.h>

namespace rsp::network {

class ResponseParser;
class EHttpRequest;

/**
 * \class HttpResponse
 *
 * \brief This is the output when making a HttpRequest and implements the IHttpResponse interface.
 *
 */
class HttpResponse: public IHttpResponse
{
public:
    explicit HttpResponse(IHttpRequest &arRequest)
        : mrRequest(arRequest)
    {
        MakeBody();
    }

    [[nodiscard]] bool IsComplete() const override
    {
        return mCompleted;
    }

    [[nodiscard]] const StatusLine& GetStatusLine() const override
    {
        return mStatusLine;
    }

    [[nodiscard]] const HeaderList& GetHeaders() const override
    {
        return mHeaders;
    }

    [[nodiscard]] std::string_view GetHeader(std::string_view aName) const override;

    [[nodiscard]] StatusCodes GetStatusCode() const override;

    [[nodiscard]] const IHttpRequest& GetRequest() const override
    {
        return mrRequest;
    }

    [[nodiscard]] IStreamDataProvider& GetBody() const override
    {
        if (!mpBody) {
            THROW_WITH_BACKTRACE1(NetworkException, "HTTP response body has not been created.");
        }
        return *mpBody;
    }

    IHttpResponse& MakeBody() override;

    [[nodiscard]] size_t GetContentLength() const override;
    IHttpResponse& Clear() override;

protected:
    friend class ResponseParser;
    IHttpRequest &mrRequest;
    std::string mHeaderData{};
    StatusLine mStatusLine{};
    HeaderList mHeaders{};
    std::optional<size_t> mContentLength{};
    HttpBody_t mpBody{};

    friend class EHttpRequest;
    bool mCompleted = false;
};

}// namespace rsp::network

#endif // RSP_CORE_LIB_NETWORK_HTTP_RESPONSE_H
