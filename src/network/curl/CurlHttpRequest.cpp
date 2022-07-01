/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <map>
#include <iterator>
#include "CurlHttpRequest.h"
#include "Exceptions.h"

using namespace rsp::network;

namespace rsp::network::curl
{

CurlHttpRequest::CurlHttpRequest()
    : mResponse(*this)
{
}

size_t CurlHttpRequest::writeFunction(void *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

size_t CurlHttpRequest::headerFunction(void *data, size_t size, size_t nmemb, HttpResponse *apResponse)
{
    std::string header(reinterpret_cast<char*>(data), size * nmemb);
    size_t seperator = header.find_first_of(':');
    if (std::string::npos == seperator) {
        trim(header);
        if (0 == header.length()) {
            return (size * nmemb); // blank line;
        }
        apResponse->AddHeader(header, "present");
    }
    else {
        std::string key = header.substr(0, seperator);
        trim(key);
        std::string value = header.substr(seperator + 1);
        trim(value);
        apResponse->AddHeader(key, value);
    }

    return (size * nmemb);
}

IHttpRequest& CurlHttpRequest::SetOptions(const HttpRequestOptions &arOptions)
{
    mRequestOptions = arOptions;
    return *this;
}

const HttpRequestOptions& CurlHttpRequest::GetOptions() const
{
    return mRequestOptions;
}

IHttpRequest& CurlHttpRequest::SetBody(const std::string &arBody)
{
    mRequestOptions.Body = arBody;
    return *this;
}

void CurlHttpRequest::Execute(std::function<void(IHttpResponse&)> callback)
{
}

IHttpResponse& CurlHttpRequest::Execute()
{
    checkRequestOptions(mRequestOptions);

    setCurlOption(CURLOPT_URL, std::string(mRequestOptions.BaseUrl + mRequestOptions.Uri).c_str());

    switch (mRequestOptions.RequestType) {
        case HttpRequestType::GET:
            break;

        case HttpRequestType::POST:
            setCurlOption(CURLOPT_POST, 1L);
            if (mRequestOptions.Body.size() > 0) {
                setCurlOption(CURLOPT_POSTFIELDS, mRequestOptions.Body.c_str());
                setCurlOption(CURLOPT_POSTFIELDSIZE, mRequestOptions.Body.size());
            }
            break;

        default:
            break;
    }

    //Redirect configuration
    setCurlOption(CURLOPT_FOLLOWLOCATION, _followRedirects);
    setCurlOption(CURLOPT_MAXREDIRS, _maxRedirects);

    //Progress and keep-alive configuration
    setCurlOption(CURLOPT_NOPROGRESS, 1L);
    setCurlOption(CURLOPT_TCP_KEEPALIVE, 1L);

    //Setup callbacks functions for Response object
    std::string response_string;
    setCurlOption(CURLOPT_WRITEFUNCTION, writeFunction);
    setCurlOption(CURLOPT_WRITEDATA, &response_string);

    setCurlOption(CURLOPT_HEADERFUNCTION, headerFunction);
    setCurlOption(CURLOPT_HEADERDATA, &mResponse);

    //Set Request headers
    std::string header;
    curl_slist *headers = NULL;
    std::map<std::string,std::string>::const_iterator iter = mRequestOptions.Headers.begin();
    for (; iter != mRequestOptions.Headers.end(); ++iter) {
        header = iter->first + ": " + iter->second;
        headers = curl_slist_append(headers, header.c_str());
    }
    setCurlOption(CURLOPT_HTTPHEADER, headers);

    //Set basic auth
    if (mRequestOptions.BasicAuthUsername.length() > 0) {
        setCurlOption(CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        setCurlOption(CURLOPT_USERPWD,
            std::string(mRequestOptions.BasicAuthUsername + ":" + mRequestOptions.BasicAuthPassword).c_str());
    }

    //Curl and evaluate
    auto res = curl_easy_perform(mpCurl);
    if (res != CURLE_OK) {
        THROW_WITH_BACKTRACE2(ECurlError, "curl_easy_perform() failed.", res);
    }

    long resp_code = 0;
    getCurlInfo(CURLINFO_RESPONSE_CODE, &resp_code);
    mResponse.PickBody(response_string).SetStatusCode(static_cast<int>(resp_code));

    return mResponse;
}

void CurlHttpRequest::checkRequestOptions(const HttpRequestOptions &arOpts)
{
    if (arOpts.RequestType == HttpRequestType::NONE || arOpts.BaseUrl == "") {
        THROW_WITH_BACKTRACE1(ERequestOptions, "Insufficient request option settings");
    }
}

} // namespace rsp::network::http

/**
 * \brief Factory function to decouple depency
 *
 * \return IHttpRequest*
 */
IHttpRequest* rsp::network::HttpRequest::MakeRequest()
{
    return new rsp::network::curl::CurlHttpRequest();
}
