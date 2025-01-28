/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_NETWORK_HTTP_DOWNLOAD_H
#define RSP_CORE_LIB_NETWORK_HTTP_DOWNLOAD_H

#include <string>
#include "IHttpRequest.h"

namespace rsp::network {

/**
 * \class HttpDownload
 * \brief Helper class to perform a download request to a destination file.
 *
 * If the file already exists, its size and hash is checked against the file on the server.
 * If the file is only partially downloaded, download will be resumed.
 */
class HttpDownload: public IHttpRequest
{
public:
    HttpDownload();
    explicit HttpDownload(const std::string &arFileName);

    /**
     * \brief Set the filename to store the download into.
     * \param arFileName
     * \return self
     */
    HttpDownload& SetFileName(const std::string &arFileName);

    [[nodiscard]] IHttpResponse& Execute() override;

    [[nodiscard]] const HttpRequestOptions& GetOptions() const override
    {
        return mPimpl->GetOptions();
    }

    HttpDownload& SetOptions(const HttpRequestOptions &arOptions) override
    {
        mPimpl->SetOptions(arOptions);
        return *this;
    }

    HttpDownload& SetBody(std::shared_ptr<IHttpBodyStream> apBody) override
    {
        mPimpl->SetBody(apBody);
        return *this;
    }

    [[nodiscard]] const IHttpBodyStream& GetBody() const override
    {
        return mPimpl->GetBody();
    }

    HttpDownload& AddField(const std::string &arFieldName, const std::string &arValue) override
    {
        mPimpl->AddField(arFieldName, arValue);
        return *this;
    }

    HttpDownload& AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile) override
    {
        mPimpl->AddFile(arFieldName, arFile);
        return *this;
    }

    std::uintptr_t GetHandle() override
    {
        return mPimpl->GetHandle();
    }

protected:
    std::unique_ptr<IHttpRequest> mPimpl;
    std::string mFileName{};

    void setFileModifiedTime(const std::string &arTimeString);
};

} /* namespace rsp::network */

#endif // RSP_CORE_LIB_NETWORK_HTTP_DOWNLOAD_H
