/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <network/FileBody.h>
#include <network/HttpRequestBase.h>
#include <posix/FileSystem.h>
#include <utils/DateTime.h>

namespace rsp::network {

IHttpResponse& HttpRequestBase::Execute()
{
    return defaultExecute();
}

IHttpResponse& HttpRequestBase::defaultExecute()
{
    if (GetOptions().RequestType != HttpRequestType::GET || !(dynamic_cast<FileBody*>(GetOptions().RequestBody.get()))) {
        return execute();
    }

    auto f_body  = GetOptions().RequestBody;
    auto &file = dynamic_cast<FileBody&>(*f_body).Get();

    std::string modified_time{};
    auto fmt = posix::FileSystem::GetFileModifiedTime(file.GetFileName());
    modified_time = fmt.ToHTTP();

    HttpRequestOptions opt = GetOptions();
    opt.RequestType = HttpRequestType::HEAD;
    opt.Headers["If-Unmodified-Since"] = modified_time; // Returns 412 if condition fails.
    opt.ResponseBody = nullptr;
    SetOptions(opt);

    IHttpResponse* resp = &(execute()); // Perform HEAD request
    if (resp->GetStatusCode() == StatusCodes::Ok) {

        // Compare local file size with server file size
        if (resp->GetContentLength() == file.GetSize()) {
            if (resp->GetHeaders().contains("last-modified") && resp->GetHeader("last-modified") == modified_time) {
                return *resp; // return HEAD response
            }
        }

        if (resp->GetHeaders().contains("accept-ranges") && resp->GetHeader("accept-ranges") != "none") {
            opt.Headers["Range"] = std::string("bytes=") + std::to_string(file.GetSize()) + "-"; // Returns 206 if range request succeeds
        }
        else {
            file.SetSize(0);
        }
    }

    if (resp->GetHeaders().contains("last-modified")) {
        modified_time = resp->GetHeader("last-modified");
        opt.Headers["If-Unmodified-Since"] = modified_time; // Fail if modified right now, after HEAD request and before GET request
    }
    else {
        opt.Headers.erase("If-Unmodified-Since");
    }

    opt.RequestType = HttpRequestType::GET;
    opt.ResponseBody = f_body;
    SetOptions(opt);

    resp = &(execute());

    constexpr StatusCodes haystack[] = {StatusCodes::Ok, StatusCodes::PartialContent};
    if (!std::ranges::contains(haystack, resp->GetStatusCode())) {
        file.SetSize(0);
    }

    file.Close();

    if (resp->GetHeaders().contains("last-modified")) {
        utils::DateTime dt(std::string(resp->GetHeader("last-modified")), utils::DateTime::Formats::HTTP);
        posix::FileSystem::SetFileModifiedTime(file.GetFileName(), dt);
    }

    return *resp;
}

} // rsp::network