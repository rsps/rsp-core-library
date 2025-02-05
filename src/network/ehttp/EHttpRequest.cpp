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

namespace rsp::network::ehttp {

EHttpRequest::EHttpRequest()
    : mResponse(*this)
{

}

const HttpRequestOptions& EHttpRequest::GetOptions() const
{
    return mOptions;
}

IHttpRequest& EHttpRequest::SetOptions(const HttpRequestOptions& arOptions)
{
    mOptions = arOptions;
    return *this;
}

IHttpRequest& EHttpRequest::SetBody(std::shared_ptr<IChunkedDataProvider> apBody)
{
    mOptions.Body = apBody;
    return *this;
}

const IChunkedDataProvider& EHttpRequest::GetBody() const
{
    return *(mOptions.Body);
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
    auto &connection = getConnection();

    mOptions.Headers["Content-Length"] = std::to_string(mOptions.Body->GetSize());

    // Send <request type> <path> <protocol>
    // Send host header
    // Send headers from options + empty line
    auto headers = formatHeaders();
    connection.Write({ reinterpret_cast<std::byte*>(headers.data()), headers.size() });

    // Send body
    ChunkStreamer<512> streamer(*mOptions.Body);
    streamer.Read();
    connection.Write()

    return mResponse;
}

uintptr_t EHttpRequest::GetHandle() const
{
    return 0;
}

SocketConnection& EHttpRequest::getConnection()
{
    if (mpSession) {
        return dynamic_cast<EHttpSession&>(*mpSession).GetConnection().Connect();
    }

    if (!mpConnection) {
        mpConnection = std::make_unique<SocketConnection>();
        mpConnection->SetOptions(mOptions);
        mpConnection->Connect();
    }

    return *mpConnection;
}

std::string EHttpRequest::formatHeaders()
{
    using namespace std::string_view_literals;
    constexpr auto cNewLine = "\r\n"sv;

    std::stringstream ss;
    ss
          << mOptions.RequestType << " "
          << mOptions.Uri
          << " HTTP/1.1\r\nHost: "sv
          << getConnection().GetHost()
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
