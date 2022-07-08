/*!
 * \copyright        Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license          Mozilla Public License 2.0
 * \author:          Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CURLHTTPREQUEST_H
#define CURLHTTPREQUEST_H

#include <network/IHttpRequest.h>
#include <network/HttpRequest.h>
#include <network/HttpResponse.h>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include "EasyCurl.h"

namespace rsp::network::curl {

class MultiCurl;

/**
 * \class CurlHttpRequest
 *
 * \brief IHttpRequest implementation using libcurl.
 */
class CurlHttpRequest: public rsp::network::IHttpRequest, public EasyCurl
{
public:
    typedef std::function<void(IHttpResponse&)> ResponseCallback_t;

    CurlHttpRequest();
    ~CurlHttpRequest() override;

    CurlHttpRequest(const CurlHttpRequest&) = default;
    CurlHttpRequest(CurlHttpRequest&&) = default;

    CurlHttpRequest& operator=(const CurlHttpRequest&) = default;
    CurlHttpRequest& operator=(CurlHttpRequest&&) = default;

    IHttpResponse& Execute() override;
    const HttpRequestOptions& GetOptions() const override;
    IHttpRequest& SetOptions(const HttpRequestOptions &arOptions) override;
    IHttpRequest& SetBody(const std::string &arBody) override;

    IHttpRequest& SetResponseHandler(ResponseCallback_t aCallback) override;
    bool IsAsync() override { return bool(mResponseHandler); }

    std::uintptr_t GetHandle() override;

    void SetData(void* apData) override
    {
        mpMultiCurl = static_cast<MultiCurl*>(apData);
    }

    void* GetData() override
    {
        return mpMultiCurl;
    }

protected:
    HttpResponse mResponse;
    HttpRequestOptions mRequestOptions{};
    ResponseCallback_t mResponseHandler{};
    MultiCurl *mpMultiCurl = nullptr;

    void requestDone() override;

private:

    static size_t writeFunction(void *ptr, size_t size, size_t nmemb, HttpResponse *data);
    static size_t headerFunction(char *data, size_t size, size_t nmemb, HttpResponse *apResponse);
    static size_t progressFunction(CurlHttpRequest *aRequest, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

    void checkRequestOptions(const HttpRequestOptions &arOpts);
    void populateOptions();

};

}

#endif
