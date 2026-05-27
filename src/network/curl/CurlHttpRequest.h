/*!
 * \copyright        Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license          Mozilla Public License 2.0
 * \author:          Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_HTTP_REQUEST_H
#define RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_HTTP_REQUEST_H

#include <network/HttpRequestBase.h>
// #include <iostream>
// #include <string>
// #include <cstring>
// #include <map>
#include <memory>
// #include <stdexcept>
// #include <utility>
#include "EasyCurl.h"
#include "CurlHttpResponse.h"

namespace rsp::network::curl {

/**
 * \class CurlHttpRequest
 *
 * \brief IHttpRequest implementation using libcurl.
 */
class CurlHttpRequest: public rsp::network::HttpRequestBase, public EasyCurl
{
public:
    CurlHttpRequest();

    CurlHttpRequest(const CurlHttpRequest&) = default;
    CurlHttpRequest(CurlHttpRequest&&) noexcept = default;

    [[nodiscard]] const HttpRequestOptions& GetOptions() const override;
    IHttpRequest& SetOptions(const HttpRequestOptions &arOptions) override;
    IHttpRequest& SetBody(HttpBody_t apBody) override;
    [[nodiscard]] const IStreamDataProvider& GetBody() const override;
    IHttpRequest& SetResponseBody(HttpBody_t apBody) override;
    [[nodiscard]] const IStreamDataProvider& GetResponseBody() const override;

    [[nodiscard]] std::uintptr_t GetHandle() const override;

protected:
    HttpRequestOptions mRequestOptions{};
    HttpBody_t mpUploadBuffer{};
    CurlHttpResponse mResponse; // Note: Must be declared after mRequestOptions injected in constructor

    IHttpResponse& execute() override;
    void readFromStream(const HttpBody_t& arBody);
    void prepareRequest() override;
    void requestDone() override;

private:
    static size_t writeFunction(void *apPtr, size_t aSize, size_t aMemberCount, const CurlHttpResponse *data);
    static size_t streamReadFunction(void *apPtr, size_t aSize, size_t aMemberCount, const IStreamDataProvider *apDataProvider);
    static size_t headerFunction(const char *apData, size_t aSize, size_t aMemberCount, CurlHttpResponse *apResponse);
    static size_t progressFunction(CurlHttpRequest *apRequest, curl_off_t aDlTotal, curl_off_t aDlNow, curl_off_t aUlTotal, curl_off_t aUlNow);

    static void checkRequestOptions(const HttpRequestOptions &arOpts);
    void populateOptions();
};

}

#endif // RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_HTTP_REQUEST_H