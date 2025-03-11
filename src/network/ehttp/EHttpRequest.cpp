/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <filesystem>
#include "EHttpRequest.h"
#include "EHttpSession.h"
#include <network/ResponseParser.h>
#include <network/parser-helpers.h>
#include <network/NetworkLibrary.h>
#include <network/MimeTypes.h>
#include <network/MultipartBody.h>
#include <network/StringBody.h>
#include <network/UrlParser.h>
#include <utils/Base64.h>

namespace rsp::network::ehttp {

EHttpRequest::EHttpRequest()
    : mResponse(*this)
{
    NetworkLibrary::Get();
}

const HttpRequestOptions& EHttpRequest::GetOptions() const
{
    return mOptions;
}

IHttpRequest& EHttpRequest::SetOptions(const HttpRequestOptions& arOptions)
{
    mOptions = arOptions;
    mResponse.Clear();
    return *this;
}

IHttpRequest& EHttpRequest::SetBody(HttpBody_t apBody)
{
    mOptions.RequestBody = apBody;
    return *this;
}

const IStreamDataProvider& EHttpRequest::GetBody() const
{
    return *(mOptions.RequestBody);
}

void EHttpRequest::prepareRequest(AutoHeaders& arHeaders)
{
    if (!mOptions.BasicAuthUsername.empty()) {
        // Add authorization header with base64 encoded credentials
        arHeaders.emplace("Authorization", "Basic " + utils::Base64::Encode(mOptions.BasicAuthUsername + ":" + mOptions.BasicAuthPassword));
    }

    if (mOptions.RequestBody) {
        if (auto p_options = dynamic_cast<MultipartBody*>(mOptions.RequestBody.get())) {
            arHeaders.emplace("Content-Type", p_options->GetBoundary().GetContentTypeHeader()); // Add header with boundary
        }

        auto len = mOptions.RequestBody->GetStreamSize();
        if (len && !mOptions.Headers.contains("content-length")) {
            arHeaders["Content-Length"] = std::to_string(len);
        }
    }
}

IHttpResponse& EHttpRequest::Execute()
{
    // Get connection
    auto &connection = getConnection().Connect();

    AutoHeaders headers;
    prepareRequest(headers);
    connection.Write(formatHeaders(headers));

    // Send body
    if (mOptions.RequestBody) {
        while (auto sz = mOptions.RequestBody->Read(mWorkBuffer)) {
            connection.Write({mWorkBuffer.data(), sz});
        }
    }

    // Read response...
    {
        ResponseParser parser(mResponse);
        while (true) {
            auto sz = connection.Read(mWorkBuffer);
            if (sz == 0) {
                break;
            }
            if (parser.ParseNewData({mWorkBuffer.data(), sz})) {
                // mResponse is now filled.
                break;
            }
        }
    }

    if (mResponseCallback) {
        mResponseCallback(mResponse);
    }

    return mResponse;
}

uintptr_t EHttpRequest::GetHandle() const
{
    return 0;
}

SocketConnection& EHttpRequest::getConnection()
{
    if (mrSession.has_value()) {
        return dynamic_cast<EHttpSession&>(mrSession.value().get()).GetConnection();
    }

    if (!mpConnection) {
        mpConnection = std::make_unique<SocketConnection>();
        mpConnection->SetOptions(mOptions);
    }

    return *mpConnection;
}

std::string EHttpRequest::formatHeaders(AutoHeaders& arHeaders)
{
    using namespace std::string_view_literals;

    std::string uri(mOptions.BaseUrl + mOptions.Uri);
    UrlParser up(uri);

    std::stringstream ss;
    ss  << mOptions.RequestType << " "
        << up.GetPath() << up.GetQuery() << up.GetFragment()
        << " HTTP/1.1\r\nHost: "sv
        << up.GetHost()
        << cNewLine;
    for (auto &h : mOptions.Headers) {
        ss << h.first << ": " << h.second << cNewLine;
    }
    for (auto &h : arHeaders) {
        ss << h.first << ": " << h.second << cNewLine;
    }
    ss << cNewLine; // Empty line before body
    return ss.str();
}

IStreamDataProvider& EHttpRequest::getRequestBody()
{
    if (!mOptions.RequestBody) {
        mOptions.RequestBody = std::make_shared<StringBody>();
    }
    return *mOptions.RequestBody;
}

} // rsp::network::ehttp


/**
 * \brief Factory function to decouple dependency
 *
 * \return IHttpRequest unique pointer
 */
std::unique_ptr<rsp::network::IHttpRequest> rsp::network::IHttpRequest::Create()
{
    return std::make_unique<rsp::network::ehttp::EHttpRequest>();
}
