/*
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 * Created Date:  Tuesday, May 17th 2022, 8:49:44 am
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * HISTORY:
 * Date      	By	Comments
 * ----------	---	----------------------------------------------------------
 */

#include "CurlHttpRequest.h"
#include <iostream>
#include <map>
#include <iterator>

using namespace rsp::network::http;
using namespace rsp::network::exceptions;

namespace rsp::network::http::curl
{

CurlHttpRequest::CurlHttpRequest()
    : mResponse(*this), mRequestOptions()
{
    checkVersion();
}

size_t CurlHttpRequest::writeFunction(void *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

size_t CurlHttpRequest::headerFunction(void *data, size_t size, size_t nmemb, void *userdata)
{
    HttpResponse *r;
    r = reinterpret_cast<HttpResponse*>(userdata);
    std::string header(reinterpret_cast<char*>(data), size * nmemb);
    size_t seperator = header.find_first_of(':');
    if (std::string::npos == seperator) {
        trim(header);
        if (0 == header.length()) {
            return (size * nmemb); // blank line;
        }
        r->AddHeader(header, "present");
    }
    else {
        std::string key = header.substr(0, seperator);
        trim(key);
        std::string value = header.substr(seperator + 1);
        trim(value);
        r->AddHeader(key, value);
    }

    return (size * nmemb);
}

IHttpRequest& CurlHttpRequest::SetOptions(HttpRequestOptions opts)
{
    mRequestOptions = opts;
    return *this;
}

IHttpRequest& CurlHttpRequest::SetHeaders(std::map<std::string,std::string> &headers)
{
    mRequestOptions.Headers = headers;
    return *this;
}

IHttpRequest& CurlHttpRequest::SetBody(std::string const &body)
{
    mRequestOptions.Body = body;
    return *this;
}

void CurlHttpRequest::Execute(std::function<void(IHttpResponse&)> callback)
{
}

IHttpResponse& CurlHttpRequest::Execute()
{
    checkRequestOptions(mRequestOptions);

    auto curl = curl_easy_init();
    if (!curl) {
        THROW_WITH_BACKTRACE1(ECurlError, "Curl initialization failed.");
    }

    curl_easy_setopt(curl, CURLOPT_URL, std::string(mRequestOptions.BaseUrl + mRequestOptions.Uri).c_str());
    switch (mRequestOptions.RequestType) {
        case HttpRequestType::GET:
            break;

        case HttpRequestType::POST:
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            if (mRequestOptions.Body.size() > 0) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, mRequestOptions.Body.c_str());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, mRequestOptions.Body.size());
            }
            break;

        default:
            break;
    }

    //Redirect configuration
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, _followRedirects);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, _maxRedirects);

    //Progress and keep-alive configuration
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    //Setup callbacks functions for Response object
    std::string response_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerFunction);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &mResponse);

    //Set Request headers
    std::string header;
    curl_slist *headers = NULL;
    std::map<std::string,std::string>::const_iterator iter = mRequestOptions.Headers.begin();
    for (; iter != mRequestOptions.Headers.end(); ++iter) {
        header = iter->first + ": " + iter->second;
        headers = curl_slist_append(headers, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    //Set basic auth
    if (mRequestOptions.BasicAuthUsername.length() > 0) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD,
            std::string(mRequestOptions.BasicAuthUsername + ":" + mRequestOptions.BasicAuthPassword).c_str());
    }

    //Curl and evaluate
    auto res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        THROW_WITH_BACKTRACE1(ECurlError, "Curl request failed with errorcode : " + *curl_easy_strerror(res));
    }
    else {
        int64_t resp_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp_code);
        mResponse.SetBody(response_string).SetStatusCode(reinterpret_cast<int&>(resp_code));
    }

    //Cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return mResponse;
}

HttpRequestOptions& CurlHttpRequest::GetOptions()
{
    return mRequestOptions;
}

void CurlHttpRequest::checkVersion()
{
    curl_version_info_data *d = curl_version_info(CURLVERSION_NOW);
    if (d->version_num < minimumCurlVersion()) {
        THROW_WITH_BACKTRACE1(ECurlVersion, "Wrong libcurl version");
    }
}

void CurlHttpRequest::checkRequestOptions(HttpRequestOptions aOpts)
{
    if (aOpts.RequestType == HttpRequestType::NONE || aOpts.BaseUrl == "") {
        THROW_WITH_BACKTRACE1(ERequestOptions, "Incorrect request option settings");
    }
}

} // namespace rsp::network::http::curl

/**
 * \brief Factory function to decouple depency
 *
 * \return IHttpRequest*
 */
IHttpRequest* rsp::network::http::HttpRequest::MakeRequest()
{
    return new rsp::network::http::curl::CurlHttpRequest();
}
