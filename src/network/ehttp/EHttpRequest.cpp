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

IHttpRequest& EHttpRequest::SetBody(std::shared_ptr<IStreamDataProvider> apBody)
{
    mOptions.Body = apBody;
    return *this;
}

const IStreamDataProvider& EHttpRequest::GetBody() const
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

    auto len = mOptions.Body->GetStreamSize();
    if (len) {
        mOptions.Headers["Content-Length"] = std::to_string(*len);
    }

    // Send <request type> <path> <protocol>
    // Send host header
    // Send headers from options + empty line
    {
        auto headers = formatHeaders();
        connection.Write({reinterpret_cast<std::byte*>(headers.data()), headers.size()});
    }

    // Send body
    {
        std::byte buffer[256];
        while (auto sz = mOptions.Body->Read(buffer)) {
            connection.Write({buffer, sz});
        }
    }

    // Read response...
    {
        ResponseParser parser(mResponse);
        auto buffer = std::array<std::byte, 256>();
        while (true) {
            auto sz = connection.Read(buffer);
            if (sz == 0) {
                break; // What if timeout?
            }
            if (parser.ParseNewData({buffer.data(), sz})) {
                // mResponse is now filled.
                break;
            }
        }
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
        return dynamic_cast<EHttpSession&>(mrSession.value().get()).GetConnection().Connect();
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
