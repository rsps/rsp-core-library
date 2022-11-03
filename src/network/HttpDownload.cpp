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
#include <utils/StrUtils.h>

using namespace rsp::posix;
using namespace rsp::utils;

namespace rsp::network {

HttpDownload::HttpDownload(const std::string &arFilename)
{
    if (FileSystem::FileExists(arFilename)) {
        auto mtime = FileSystem::GetFileModifiedTime(arFilename);
        mModifiedTime = StrUtils::TimeStamp(mtime, StrUtils::TimeFormats::HTTP);
    }

    mFile.Open(arFilename, std::ios::in | std::ios::out, 0640);

    WriteToFile(mFile);
}

IHttpResponse& HttpDownload::Execute()
{
    HttpRequestOptions orig_opt = GetOptions();
    orig_opt.RequestType = HttpRequestType::GET;

    HttpRequestOptions opt = orig_opt;
    if (mFile.GetSize() > 0) {
        opt.Headers["Range"] = std::string("bytes=") + std::to_string(mFile.GetSize()) + "-";
    }
    if (!mModifiedTime.empty()) {
        opt.Headers["If-Unmodified-Since"] = mModifiedTime;
        // Returns 412 if condition fails.
    }
    SetOptions(opt);

    IHttpResponse& resp = mPimpl->Execute();

//    if (resp.GetStatusCode() != 200) {
//        SetOptions(orig_opt);
//        return mPimpl->Execute();
//    }

    return resp;
}

} /* namespace rsp::network */
