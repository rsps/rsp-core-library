/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "EHttpRequest.h"
#include "EHttpSession.h"
#include <network/ChunkStreamer.h>
#include <network/ResponseParser.h>
#include <network/parser-helpers.h>
#include <network/NetworkLibrary.h>
#include <network/UrlParser.h>

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

IHttpRequest& EHttpRequest::SetBody(std::shared_ptr<IStreamDataProvider> apBody)
{
    mOptions.RequestBody = apBody;
    return *this;
}

const IStreamDataProvider& EHttpRequest::GetBody() const
{
    return *(mOptions.RequestBody);
}

IHttpRequest& EHttpRequest::AddField(const std::string& arFieldName, const std::string& arValue)
{
    return *this;
}

IHttpRequest& EHttpRequest::AddFile(const std::string& arFieldName, posix::FileIO& arFile)
{
    return *this;
}

IHttpResponse& EHttpRequest::Execute()
{
    // Get connection
    auto &connection = getConnection().Connect();

    if (mOptions.RequestBody) {
        auto len = mOptions.RequestBody->GetStreamSize();
        if (len) {
            mOptions.Headers["Content-Length"] = std::to_string(*len);
        }
    }

    // Send <request type> <path> <protocol>
    // Send host header
    // Send headers from options + empty line
    {
        auto headers = formatHeaders();
        connection.Write({reinterpret_cast<std::byte*>(headers.data()), headers.size()});
    }

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
                break; // TODO: Test for timeout...
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

std::string EHttpRequest::formatHeaders()
{
    using namespace std::string_view_literals;

    std::string uri(mOptions.BaseUrl + mOptions.Uri);
    UrlParser up(uri);

    std::stringstream ss;
    ss
          << mOptions.RequestType << " "
          << up.GetPath() << up.GetQuery() << up.GetFragment()
          << " HTTP/1.1\r\nHost: "sv
          << up.GetHost()
          << cNewLine;
    for (auto &h : mOptions.Headers) {
        ss << h.first << ": " << h.second << cNewLine;
    }
    ss << cNewLine; // Empty line before body

    return ss.str();
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
