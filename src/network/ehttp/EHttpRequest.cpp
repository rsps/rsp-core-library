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
#include <network/FileBody.h>
#include <network/MimeTypes.h>
#include <network/StringBody.h>
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

IHttpRequest& EHttpRequest::SetBody(HttpBody_t apBody)
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
    mMultipartFormType = true;

    std::string s = mBoundary.MakeContentDisposition(arFieldName);
    s += arValue;
    getRequestBody().Write({ reinterpret_cast<const std::byte*>(s.data()), s.size()});

    return *this;
}

IHttpRequest& EHttpRequest::AddFile(const std::string& arFieldName, posix::FileIO& arFile)
{
    mMultipartFormType = true;

    auto &body = getRequestBody();

    std::filesystem::path file = arFile.GetFileName();
    std::string ext = file.extension();
    std::string mime_type(MimeTypes::GetType(ext));

    std::string s = mBoundary.MakeContentDisposition(arFieldName, file.filename(), mime_type);
    body.Write({ reinterpret_cast<const std::byte*>(s.data()), s.size()});
    FileBody fb(arFile);
    body.CopyFrom(fb);

    return *this;
}

IHttpResponse& EHttpRequest::Execute()
{
    // Get connection
    auto &connection = getConnection().Connect();

    if (!mOptions.BasicAuthUsername.empty()) {
        mOptions.Headers.emplace("Authorization", "Basic " + mOptions.BasicAuthPassword);
    }

    if (mMultipartFormType) {
        mOptions.Headers.emplace("Content-Type", mBoundary.GetContentTypeHeader());
        std::string s = mBoundary.GetEndBoundary();
        mOptions.RequestBody->Write({ reinterpret_cast<const std::byte*>(s.data()), s.size()});
    }

    if (mOptions.RequestBody) {
        auto len = mOptions.RequestBody->GetStreamSize();
        if (len) {
            mOptions.Headers["Content-Length"] = std::to_string(len);
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
