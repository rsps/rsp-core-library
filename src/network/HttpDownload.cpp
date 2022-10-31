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

namespace rsp::network {

HttpDownload::HttpDownload(const std::string &arFilename)
    : mFile(arFilename, std::ios::in | std::ios::out, 0640)
{
}

IHttpResponse& HttpDownload::Execute()
{
    // TODO: If file exists, first make head request and verify if file is correct and finished.
    // TODO: OR execute with etag in "If-None-Match" header and ranges set to "file size-"
    HttpRequestOptions opt = GetOptions();
    opt.RequestType = HttpRequestType::GET;
    opt.Headers["Range"] = std::string("bytes=") + std::to_string(mFile.GetSize()) + "-";
    opt.Headers["ETag"] = ""; // TODO: Add etag of file here if known.
    SetOptions(opt);

//    IHttpResponse &resp = mPimpl->Execute();
//    if (resp.GetStatusCode() == 200) {
//        if (resp.GetHeaders().at("content-length") == mFile.GetSize()) {
//            // TODO: Check etag hash, check Aretaeus documentation
//        }
//        else {
//
//        }
////            content-type: image/png
////            etag: "4284873773"
////            last-modified: Tue, 23 Aug 2022 13:03:39 GMT
//
//    }


    // Start over or continue according to result of file check.
    return mPimpl->Execute();
}

} /* namespace rsp::network */
