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
#include <posix/FileIO.h>
#include "CurlHttpRequest.h"
#include "CurlSession.h"
#include "Exceptions.h"
#include <utils/StrUtils.h>
#include <logging/Logger.h>

using namespace rsp::logging;
using namespace rsp::network;
using namespace rsp::utils;

namespace rsp::network::curl
{

CurlHttpRequest::CurlHttpRequest()
    : mResponse(*this)
{
    //Setup default callback functions for Response object
    setCurlOption(CURLOPT_WRITEFUNCTION, writeFunction);
    setCurlOption(CURLOPT_WRITEDATA, &mResponse);

    setCurlOption(CURLOPT_HEADERFUNCTION, headerFunction);
    setCurlOption(CURLOPT_HEADERDATA, &mResponse);

    setCurlOption(CURLOPT_XFERINFOFUNCTION, progressFunction);
    setCurlOption(CURLOPT_XFERINFODATA, this);
}


CurlHttpRequest::~CurlHttpRequest()
{
}

size_t CurlHttpRequest::writeFunction(void *ptr, size_t size, size_t nmemb, HttpResponse *apResponse)
{
    apResponse->GetBody().append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

size_t CurlHttpRequest::headerFunction(char *data, size_t size, size_t nmemb, HttpResponse *apResponse)
{
    std::string header(data, size * nmemb);
    size_t seperator = header.find_first_of(':');
    if (std::string::npos == seperator) {
        StrUtils::Trim(header);
        if (0 == header.length()) {
            return (size * nmemb); // blank line;
        }
        apResponse->AddHeader(StrUtils::ToLower(header), "present");
    }
    else {
        std::string key = header.substr(0, seperator);
        StrUtils::Trim(key);
        std::string value = header.substr(seperator + 1);
        StrUtils::Trim(value);
        apResponse->AddHeader(StrUtils::ToLower(key), value);
    }

    return (size * nmemb);
}

size_t CurlHttpRequest::progressFunction(CurlHttpRequest *aRequest, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    // TODO: Call progress callback on request here...

    return CURLE_OK;
}

IHttpRequest& CurlHttpRequest::SetOptions(const HttpRequestOptions &arOptions)
{
    mRequestOptions = arOptions;
//    curl_easy_reset(mpCurl);
    populateOptions();
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

IHttpResponse& CurlHttpRequest::Execute()
{
    checkRequestOptions(mRequestOptions);

    //Curl and evaluate
    auto res = curl_easy_perform(mpCurl);
    if (res != CURLE_OK) {
        THROW_WITH_BACKTRACE2(ECurlError, "curl_easy_perform() failed.", res);
    }

    requestDone();

    return mResponse;
}

void CurlHttpRequest::checkRequestOptions(const HttpRequestOptions &arOpts)
{
    if (arOpts.RequestType == HttpRequestType::NONE || arOpts.BaseUrl == "") {
        THROW_WITH_BACKTRACE1(ERequestOptions, "Insufficient request option settings");
    }
}

void CurlHttpRequest::requestDone()
{
    long resp_code = 0;
    getCurlInfo(CURLINFO_RESPONSE_CODE, &resp_code);
    mResponse.SetStatusCode(static_cast<int>(resp_code));

    Logger::GetDefault().Debug() << "Request to " << mRequestOptions.BaseUrl << mRequestOptions.Uri << " is finished with code " << resp_code << std::endl;
}

std::uintptr_t CurlHttpRequest::GetHandle()
{
    return std::uintptr_t(mpCurl);
}

void CurlHttpRequest::populateOptions()
{
    setCurlOption(CURLOPT_URL, std::string(mRequestOptions.BaseUrl + mRequestOptions.Uri).c_str());

    switch (mRequestOptions.RequestType) {
        case HttpRequestType::GET:
            setCurlOption(CURLOPT_HTTPGET, 1L);
            break;

        case HttpRequestType::POST:
            setCurlOption(CURLOPT_POST, 1L);
            if (mRequestOptions.Body.size() > 0) {
                setCurlOption(CURLOPT_POSTFIELDS, mRequestOptions.Body.c_str());
                setCurlOption(CURLOPT_POSTFIELDSIZE, mRequestOptions.Body.size());
            }
            break;

        case HttpRequestType::HEAD:
            setCurlOption(CURLOPT_NOBODY, 1L);
            break;

        case HttpRequestType::PATCH:
            setCurlOption(CURLOPT_CUSTOMREQUEST, "PATCH");
            break;

        case HttpRequestType::PUT:
            setCurlOption(CURLOPT_PUT, 1L);
            break;

        case HttpRequestType::DELETE:
            setCurlOption(CURLOPT_CUSTOMREQUEST, "DELETE");
            break;

        default:
            break;
    }

    setCurlOption(CURLOPT_VERBOSE, mRequestOptions.Verbose);

    //Redirect configuration
    setCurlOption(CURLOPT_FOLLOWLOCATION, _followRedirects);
    setCurlOption(CURLOPT_MAXREDIRS, _maxRedirects);

    //Progress and keep-alive configuration
    setCurlOption(CURLOPT_NOPROGRESS, 1L);
    setCurlOption(CURLOPT_TCP_KEEPALIVE, 1L);

    setCurlOption(CURLOPT_CONNECTTIMEOUT, mRequestOptions.ConnectionTimeout);
    setCurlOption(CURLOPT_SERVER_RESPONSE_TIMEOUT, mRequestOptions.ResponseTimeout);

    setCurlOption(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

//    CURLMOPT_PIPELINING to CURLPIPE_MULTIPLEX
//    CURLOPT_PIPEWAIT

    if (!mRequestOptions.CertCaPath.empty()) {
        setCurlOption(CURLOPT_CAINFO, mRequestOptions.CertCaPath.c_str());
    }

    if (!mRequestOptions.CertPath.empty()) {
        setCurlOption(CURLOPT_SSLCERT, mRequestOptions.CertPath.c_str());
        setCurlOption(CURLOPT_SSLKEY, mRequestOptions.KeyPath.c_str());
        setCurlOption(CURLOPT_KEYPASSWD, mRequestOptions.KeyPasswd.c_str());
    }

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
