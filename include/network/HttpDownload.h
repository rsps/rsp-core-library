/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_NETWORK_HTTPDOWNLOAD_H_
#define INCLUDE_NETWORK_HTTPDOWNLOAD_H_

#include <string>
#include <network/HttpRequest.h>
#include <posix/FileIO.h>

namespace rsp::network {

/**
 * \class HttpDownload
 * \brief Helper class to perform a download request to a destination file.
 *
 * If the file already exists, its size and hash is checked against the file on the server.
 * If the file is only partially downloaded, download will be resumed.
 */
class HttpDownload: public HttpRequest
{
public:
    HttpDownload(const std::string &arFilename);

    IHttpResponse& Execute() override;

protected:
    std::string mModifiedTime{};
    rsp::posix::FileIO mFile{};
};

} /* namespace rsp::network */

#endif /* INCLUDE_NETWORK_HTTPDOWNLOAD_H_ */
