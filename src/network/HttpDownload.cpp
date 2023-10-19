/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <string>
#include <network/HttpDownload.h>
#include <posix/FileSystem.h>
#include <posix/FileIO.h>
#include <utils/DateTime.h>

using namespace rsp::posix;
using namespace rsp::utils;

namespace rsp::network {

HttpDownload::HttpDownload(const std::string &arFileName)
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
        return HttpRequest::Execute();
    }

    std::string modified_time{};
    if (FileSystem::FileExists(mFileName)) {
        auto mtime = FileSystem::GetFileModifiedTime(mFileName);
        modified_time = mtime.ToHTTP();
    }

    rsp::posix::FileIO file(mFileName, std::ios::in | std::ios::out | std::ios_base::ate, 0640);

    HttpRequestOptions orig_opt = GetOptions();
    orig_opt.RequestType = HttpRequestType::GET;

    HttpRequestOptions opt = orig_opt;
    opt.WriteFile = &file; // Redirect response body to file
    opt.Headers["Range"] = std::string("bytes=") + std::to_string(file.GetSize()) + "-"; // Returns 206 if range request succeeds
    if (!modified_time.empty()) {
        opt.Headers["If-Unmodified-Since"] = modified_time; // Returns 412 if condition fails.
    }
    SetOptions(opt);

    IHttpResponse* resp = &(mPimpl->Execute());

    if (resp->GetStatusCode() != 206) {
        file.SetSize(0);
        SetOptions(orig_opt);
        resp = &(mPimpl->Execute());
    }

    file.Close();
    SetFileModifiedTime(resp->GetHeader("last-modified"));

    return *resp;
}

void HttpDownload::SetFileModifiedTime(const std::string &arTimeString)
{
    using namespace std::chrono;
    DateTime dt(arTimeString, DateTime::Formats::HTTP);
    FileSystem::SetFileModifiedTime(mFileName, dt);
}

} /* namespace rsp::network */
