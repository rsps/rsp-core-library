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

    IHttpResponse& Execute() override
    {
        return mPimpl->Execute();
    }

    IHttpRequest& SetResponseHandler(std::function<void(IHttpResponse&)> aCallback) override
    {
        mPimpl->SetResponseHandler(aCallback);
        return *this;
    }

    bool IsAsync() override
    {
        return mPimpl->IsAsync();
    }

    std::uintptr_t GetHandle() override
    {
        return mPimpl->GetHandle();
    }

    void SetData(void* apData) override
    {
        mPimpl->SetData(apData);
    }

    void* GetData() override
    {
        return mPimpl->GetData();
    }

protected:
    std::unique_ptr<IHttpRequest> mPimpl;
    static IHttpRequest* MakeRequest();
};

} // rsp::network

#endif
