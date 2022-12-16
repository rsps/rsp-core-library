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
#include <string>
#include <logging/Logger.h>
#include <network/HttpRequest.h>
#include <posix/FileIO.h>
#include <utils/StrUtils.h>
#include "CurlHttpRequest.h"
#include "CurlSession.h"
#include "Exceptions.h"

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

IHttpRequest& CurlHttpRequest::WriteToFile(rsp::posix::FileIO &arFile)
{
    setCurlOption(CURLOPT_WRITEFUNCTION, fileWriteFunction);
    setCurlOption(CURLOPT_WRITEDATA, &arFile);
    return *this;
}

IHttpRequest& CurlHttpRequest::ReadFromFile(rsp::posix::FileIO &arFile)
{
    setCurlOption(CURLOPT_UPLOAD, 1L);
    setCurlOption(CURLOPT_READFUNCTION, fileReadFunction);
    setCurlOption(CURLOPT_READDATA, &arFile);
    setCurlOption(CURLOPT_INFILESIZE_LARGE, static_cast<unsigned long>(arFile.GetSize()));
    return *this;
}

IHttpRequest& CurlHttpRequest::ReadFromString(const std::string &arString)
{
    setCurlOption(CURLOPT_UPLOAD, 1L);
    setCurlOption(CURLOPT_READFUNCTION, stringReadFunction);
    mUploadBuffer.Remaining = arString.size();
    mUploadBuffer.Data = arString.c_str();
    setCurlOption(CURLOPT_READDATA, &mUploadBuffer);
    setCurlOption(CURLOPT_INFILESIZE_LARGE, static_cast<unsigned long>(arString.size()));
    return *this;
}

size_t CurlHttpRequest::writeFunction(void *ptr, size_t size, size_t nmemb, CurlHttpResponse *apResponse)
{
    apResponse->getBody().append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

size_t CurlHttpRequest::fileWriteFunction(void *ptr, size_t size, size_t nmemb, rsp::posix::FileIO *apFile)
{
    return apFile->Write(ptr, size * nmemb);
}

size_t CurlHttpRequest::fileReadFunction(void *ptr, size_t size, size_t nmemb, rsp::posix::FileIO *apFile)
{
    return apFile->Read(ptr, size * nmemb);
}

size_t CurlHttpRequest::stringReadFunction(void *ptr, size_t size, size_t nmemb, UploadBuffer *apBuf)
{
    size_t sz = size * nmemb;
    if (sz > apBuf->Remaining) {
        sz = apBuf->Remaining;
    }
//    Logger::GetDefault().Debug() << "Copying " << sz << " characters to network buffer" << std::endl;
    std::memcpy(ptr, apBuf->Data, sz);
    apBuf->Data += sz;
    apBuf->Remaining -= sz;
    return sz;
}

size_t CurlHttpRequest::headerFunction(char *data, size_t size, size_t nmemb, CurlHttpResponse *apResponse)
{
    std::string header(data, size * nmemb);
    size_t seperator = header.find_first_of(':');
    if (std::string::npos == seperator) {
        StrUtils::Trim(header);
        if (0 == header.length()) {
            return (size * nmemb); // blank line;
        }
        apResponse->addHeader(StrUtils::ToLower(header), "present");
    }
    else {
        std::string key = header.substr(0, seperator);
        StrUtils::Trim(key);
        std::string value = header.substr(seperator + 1);
        StrUtils::Trim(value);
        apResponse->addHeader(StrUtils::ToLower(key), value);
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

const std::string& CurlHttpRequest::GetBody() const
{
    return mRequestOptions.Body;
}


IHttpRequest& CurlHttpRequest::AddField(const std::string &arFieldName, const std::string &arValue)
{
    curl_mimepart *field = curl_mime_addpart(getForm());
    if (field == nullptr) {
        THROW_WITH_BACKTRACE1(ECurlError, "curl_mime_addpart() failed.");
    }
    curl_mime_name(field, arFieldName.c_str());
    curl_mime_data(field, arValue.c_str(), CURL_ZERO_TERMINATED);
    return *this;
}

IHttpRequest& CurlHttpRequest::AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile)
{
    curl_mimepart *field = curl_mime_addpart(getForm());
    if (field == nullptr) {
        THROW_WITH_BACKTRACE1(ECurlError, "curl_mime_addpart() failed.");
    }
    curl_mime_name(field, arFieldName.c_str());
    curl_mime_filedata(field, arFile.GetFileName().c_str());
    return *this;
}

IHttpResponse& CurlHttpRequest::Execute()
{
    prepareRequest();

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

void CurlHttpRequest::prepareRequest()
{
    EasyCurl::prepareRequest();
    checkRequestOptions(mRequestOptions);
    populateOptions();
    mResponse.clear();
}

void CurlHttpRequest::requestDone()
{
    long resp_code = 0;
    getCurlInfo(CURLINFO_RESPONSE_CODE, &resp_code);
    mResponse.setStatusCode(static_cast<int>(resp_code));

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
            if (hasForm()) {
                setCurlOption(CURLOPT_POST, 1L);
                setCurlOption(CURLOPT_MIMEPOST, getForm());
            }
            else if (mRequestOptions.Body.size() > 0) {
                setCurlOption(CURLOPT_CUSTOMREQUEST, "POST");
                if (mRequestOptions.Body.size() > 0) {
                    ReadFromString(mRequestOptions.Body);
                }
//                setCurlOption(CURLOPT_POSTFIELDS, mRequestOptions.Body.c_str());
//                setCurlOption(CURLOPT_POSTFIELDSIZE, mRequestOptions.Body.size());
            }
            break;

        case HttpRequestType::HEAD:
            setCurlOption(CURLOPT_NOBODY, 1L);
            break;

        case HttpRequestType::PATCH:
            setCurlOption(CURLOPT_CUSTOMREQUEST, "PATCH");
            if (mRequestOptions.Body.size() > 0) {
                ReadFromString(mRequestOptions.Body);
            }
            break;

        case HttpRequestType::PUT:
            // setCurlOption(CURLOPT_PUT, 1L); // Seems to put files only
            setCurlOption(CURLOPT_CUSTOMREQUEST, "PUT");
            if (mRequestOptions.Body.size() > 0) {
                ReadFromString(mRequestOptions.Body);
            }
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
        if (!mRequestOptions.KeyPasswd.empty()) {
            setCurlOption(CURLOPT_KEYPASSWD, mRequestOptions.KeyPasswd.c_str());
        }
    }

    //Set Request headers
    if (mpHeaders) {
        curl_slist_free_all(mpHeaders);
        mpHeaders = nullptr;
    }
    for (auto const& tuple : mRequestOptions.Headers) {
        std::string header = tuple.first + ": " + tuple.second;
        Logger::GetDefault().Debug() << "Add header: " << header << std::endl;
        auto *temp = curl_slist_append(mpHeaders, header.c_str());
        if (temp == nullptr) {
            curl_slist_free_all(mpHeaders);
            mpHeaders = nullptr;
            THROW_WITH_BACKTRACE1(ECurlError, "curl_slist_free_all failed.");
        }
        mpHeaders = temp;
    }
    if (mpHeaders) {
        setCurlOption(CURLOPT_HTTPHEADER, mpHeaders);
    }

    //Set basic auth
    if (mRequestOptions.BasicAuthUsername.length() > 0) {
        setCurlOption(CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        setCurlOption(CURLOPT_USERPWD,
            std::string(mRequestOptions.BasicAuthUsername + ":" + mRequestOptions.BasicAuthPassword).c_str());
    }
}

} // namespace rsp::network::http

/**
 * \brief Factory function to decouple dependency
 *
 * \return IHttpRequest*
 */
IHttpRequest* rsp::network::HttpRequest::MakeRequest()
{
    return new rsp::network::curl::CurlHttpRequest();
}
