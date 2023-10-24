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

#include <network/IHttpRequest.h>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include "EasyCurl.h"
#include "CurlHttpResponse.h"

namespace rsp::network::curl {

/**
 * \class CurlHttpRequest
 *
 * \brief IHttpRequest implementation using libcurl.
 */
class CurlHttpRequest: public rsp::network::IHttpRequest, public EasyCurl
{
public:
    CurlHttpRequest();

    CurlHttpRequest(const CurlHttpRequest&) = default;
    CurlHttpRequest(CurlHttpRequest&&) noexcept = default;

    IHttpResponse& Execute() override;
    [[nodiscard]] const HttpRequestOptions& GetOptions() const override;
    IHttpRequest& SetOptions(const HttpRequestOptions &arOptions) override;
    IHttpRequest& SetBody(const std::string &arBody) override;
    [[nodiscard]] const std::string& GetBody() const override;

    IHttpRequest& AddField(const std::string &arFieldName, const std::string &arValue) override;
    IHttpRequest& AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile) override;

    std::uintptr_t GetHandle() override;

protected:
    CurlHttpResponse mResponse;
    HttpRequestOptions mRequestOptions{};
    struct UploadBuffer {
        size_t Remaining = 0;
        const char* Data = nullptr;
    };
    UploadBuffer mUploadBuffer{};

    void writeToFile(rsp::posix::FileIO *apFile);
    void readFromFile(rsp::posix::FileIO *apFile);
    void readFromString(const std::string &arString);

    void prepareRequest() override;
    void requestDone() override;

private:

    static size_t writeFunction(void *ptr, size_t size, size_t nmemb, CurlHttpResponse *data);
    static size_t fileWriteFunction(void *ptr, size_t size, size_t nmemb, rsp::posix::FileIO *apFile);
    static size_t fileReadFunction(void *ptr, size_t size, size_t nmemb, rsp::posix::FileIO *apFile);
    static size_t stringReadFunction(void *ptr, size_t size, size_t nmemb, UploadBuffer *apBuf);
    static size_t headerFunction(char *data, size_t size, size_t nmemb, CurlHttpResponse *apResponse);
    static size_t progressFunction(CurlHttpRequest *aRequest, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

    void checkRequestOptions(const HttpRequestOptions &arOpts);
    void populateOptions();

};

}

#endif // RSP_CORE_LIB_SRC_NETWORK_CURL_CURL_HTTP_REQUEST_H