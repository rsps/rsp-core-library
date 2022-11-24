/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <network/IHttpRequest.h>
#include <network/HttpResponse.h>

namespace rsp::network {

class MultiCurl;

class HttpRequest: public IHttpRequest
{
public:
    HttpRequest();
    HttpRequest(const HttpRequestOptions& arOptions);

    IHttpRequest& WriteToFile(rsp::posix::FileIO &arFile) override
    {
        return mPimpl->WriteToFile(arFile);
    }

    IHttpRequest& ReadFromFile(rsp::posix::FileIO &arFile) override
    {
        return mPimpl->ReadFromFile(arFile);
    }

    const HttpRequestOptions& GetOptions() const override
    {
        return mPimpl->GetOptions();
    }

    IHttpRequest& SetOptions(const HttpRequestOptions &arOptions) override
    {
        mPimpl->SetOptions(arOptions);
        return *this;
    }

    IHttpRequest& SetBody(std::string const &body) override
    {
        mPimpl->SetBody(body);
        return *this;
    }

    IHttpRequest& AddField(const std::string &arFieldName, const std::string &arValue) override
    {
        return mPimpl->AddField(arFieldName, arValue);
    }

    IHttpRequest& AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile) override
    {
        ReadFromFile(arFile);
        return mPimpl->AddFile(arFieldName, arFile);
    }

    IHttpResponse& Execute() override
    {
        return mPimpl->Execute();
    }

    std::uintptr_t GetHandle() override
    {
        return mPimpl->GetHandle();
    }

protected:
    std::unique_ptr<IHttpRequest> mPimpl;
    static IHttpRequest* MakeRequest();
};

} // rsp::network

#endif
