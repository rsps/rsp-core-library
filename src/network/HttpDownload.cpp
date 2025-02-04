/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <algorithm>
#include <list>
#include <string>
#include <network/HttpDownload.h>
#include <posix/FileSystem.h>
#include <posix/FileIO.h>
#include <utils/DateTime.h>

using namespace rsp::posix;
using namespace rsp::utils;

namespace rsp::network {

HttpDownload::HttpDownload(const std::string &arFileName)
    : HttpRequest()
{
    SetFileName(arFileName);
}

HttpDownload& HttpDownload::SetFileName(const std::string &arFileName)
{
    mFileName = arFileName;
    return *this;
}

IHttpResponse& HttpDownload::Execute()
{
    if (mFileName.empty()) {
        return mPimpl->Execute();
    }

    std::string modified_time{};
    if (FileSystem::FileExists(mFileName)) {
        auto fmt = FileSystem::GetFileModifiedTime(mFileName);
        modified_time = fmt.ToHTTP();
    }

    HttpRequestOptions opt = GetOptions();
    opt.RequestType = HttpRequestType::HEAD;
    if (!modified_time.empty()) {
        opt.Headers["If-Unmodified-Since"] = modified_time; // Returns 412 if condition fails.
    }
    SetOptions(opt);

    IHttpResponse* resp = &(mPimpl->Execute());
    if (resp->GetStatusCode() != StatusCodes::Ok) {
        return *resp;
    }

    rsp::posix::FileIO file(mFileName, std::ios::in | std::ios::out | std::ios_base::ate, 0640);

    if (resp->GetHeaders().contains("content-length") && std::stoul(resp->GetHeader("content-length")) == file.GetSize()) {
        if (resp->GetHeaders().contains("last-modified") && resp->GetHeader("last-modified") == modified_time) {
            return *resp;
        }
    }

    if (resp->GetHeaders().contains("accept-ranges") && resp->GetHeader("accept-ranges") != "none") {
        opt.Headers["Range"] = std::string("bytes=") + std::to_string(file.GetSize()) + "-"; // Returns 206 if range request succeeds
    }
    else {
        file.SetSize(0);
    }
    opt.RequestType = HttpRequestType::GET;
    opt.WriteFile = file; // Redirect response body to file
    SetOptions(opt);

    resp = &(mPimpl->Execute());

    constexpr StatusCodes haystack[] = {StatusCodes::Ok, StatusCodes::PartialContent};
    if (!std::ranges::contains(haystack, resp->GetStatusCode())) {
        file.SetSize(0);
    }

    file.Close();
    if (resp->GetHeaders().contains("last-modified")) {
        setFileModifiedTime(resp->GetHeader("last-modified"));
    }

    return *resp;
}

void HttpDownload::setFileModifiedTime(const std::string &arTimeString)
{
    DateTime dt(arTimeString, DateTime::Formats::HTTP);
    FileSystem::SetFileModifiedTime(mFileName, dt);
}

} /* namespace rsp::network */
