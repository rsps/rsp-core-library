/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <map>
#include <string>
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
}

void CurlHttpRequest::writeToFile(rsp::posix::FileIO* apFile)
{
    setCurlOption(CURLOPT_WRITEFUNCTION, fileWriteFunction);
    setCurlOption(CURLOPT_WRITEDATA, apFile);
}

void CurlHttpRequest::readFromFile(rsp::posix::FileIO* apFile)
{
    setCurlOption(CURLOPT_UPLOAD, 1L);
    setCurlOption(CURLOPT_READFUNCTION, fileReadFunction);
    setCurlOption(CURLOPT_READDATA, apFile);
    setCurlOption(CURLOPT_INFILESIZE_LARGE, static_cast<unsigned long>(apFile->GetSize()));
}

void CurlHttpRequest::readFromString(const std::string &arString)
{
    setCurlOption(CURLOPT_UPLOAD, 1L);
    setCurlOption(CURLOPT_READFUNCTION, stringReadFunction);
    mUploadBuffer.String = { arString.size(), arString.c_str() };
    setCurlOption(CURLOPT_READDATA, &mUploadBuffer);
    setCurlOption(CURLOPT_INFILESIZE_LARGE, static_cast<unsigned long>(arString.size()));
}

void CurlHttpRequest::readFromStream(const std::shared_ptr<IHttpBodyStream>& arBody)
{
    if (!arBody) {
        return;
    }
    if (arBody->GetSize() == 0) {
        return;
    }
    setCurlOption(CURLOPT_UPLOAD, 1L);
    setCurlOption(CURLOPT_READFUNCTION, streamReadFunction);
    mUploadBuffer.Stream = { 0, 0, arBody.get() };
    setCurlOption(CURLOPT_READDATA, &mUploadBuffer);
    setCurlOption(CURLOPT_INFILESIZE_LARGE, arBody->GetSize());
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
    if (sz > apBuf->String.Remaining) {
        sz = apBuf->String.Remaining;
    }
//    mLogger.Debug() << "Copying " << sz << " characters to network buffer";
    std::memcpy(ptr, apBuf->String.Data, sz);
    apBuf->String.Data += sz;
    apBuf->String.Remaining -= sz;
    return sz;
}

size_t CurlHttpRequest::streamReadFunction(void *ptr, size_t size, size_t nmemb, CurlHttpRequest::UploadBuffer *apBuf)
{
    size_t written;
    (void) apBuf->Stream.Body->GetChunk(static_cast<char*>(ptr), size * nmemb, written, apBuf->Stream.ChunkIndex, apBuf->Stream.PayloadIndex);
    return written;
}

size_t CurlHttpRequest::headerFunction(char *data, size_t size, size_t nmemb, CurlHttpResponse *apResponse)
{
    std::string header(data, size * nmemb);
    size_t separator = header.find_first_of(':');
    if (std::string::npos == separator) {
        StrUtils::Trim(header);
        if (header.empty()) {
            return (size * nmemb); // blank line;
        }
        apResponse->addHeader(StrUtils::ToLower(header), "present");
    }
    else {
        std::string key = header.substr(0, separator);
        StrUtils::Trim(key);
        std::string value = header.substr(separator + 1);
        StrUtils::Trim(value);
        apResponse->addHeader(StrUtils::ToLower(key), value);
    }

    return (size * nmemb);
}

size_t CurlHttpRequest::progressFunction(CurlHttpRequest */*aRequest*/, curl_off_t /*dltotal*/, curl_off_t /*dlnow*/, curl_off_t /*ultotal*/, curl_off_t /*ulnow*/)
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

IHttpRequest& CurlHttpRequest::SetBody(std::shared_ptr<IHttpBodyStream> apBody)
{
    mRequestOptions.Body = apBody;
    return *this;
}

const IHttpBodyStream& CurlHttpRequest::GetBody() const
{
    return *mRequestOptions.Body;
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
    if (arOpts.RequestType == HttpRequestType::NONE || arOpts.BaseUrl.empty()) {
        THROW_WITH_BACKTRACE1(ERequestOptions, "Insufficient request option settings");
    }
}

void CurlHttpRequest::prepareRequest()
{
    EasyCurl::prepareRequest();
    //Setup default callback functions for Response object
    setCurlOption(CURLOPT_WRITEFUNCTION, writeFunction);
    setCurlOption(CURLOPT_WRITEDATA, &mResponse);

    setCurlOption(CURLOPT_HEADERFUNCTION, headerFunction);
    setCurlOption(CURLOPT_HEADERDATA, &mResponse);

    setCurlOption(CURLOPT_XFERINFOFUNCTION, progressFunction);
    setCurlOption(CURLOPT_XFERINFODATA, this);

    checkRequestOptions(mRequestOptions);
    populateOptions();
    mResponse.clear();
}

void CurlHttpRequest::requestDone()
{
    long resp_code = 0;
    getCurlInfo(CURLINFO_RESPONSE_CODE, &resp_code);
    mResponse.setStatusCode(static_cast<int>(resp_code));

    mLogger.Debug() << "Request to " << mRequestOptions.BaseUrl << mRequestOptions.Uri << " is finished with code " << resp_code;

    EasyCurl::requestDone();
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
            else {
                setCurlOption(CURLOPT_CUSTOMREQUEST, "POST");
                readFromStream(mRequestOptions.Body);
            }
            break;

        case HttpRequestType::HEAD:
            setCurlOption(CURLOPT_NOBODY, 1L);
            break;

        case HttpRequestType::PATCH:
            setCurlOption(CURLOPT_CUSTOMREQUEST, "PATCH");
            readFromStream(mRequestOptions.Body);
            break;

        case HttpRequestType::PUT:
            // setCurlOption(CURLOPT_PUT, 1L); // Seems to put files only
            setCurlOption(CURLOPT_CUSTOMREQUEST, "PUT");
            readFromStream(mRequestOptions.Body);
            break;

        case HttpRequestType::DELETE:
            setCurlOption(CURLOPT_CUSTOMREQUEST, "DELETE");
            break;

        default:
            break;
    }

    if (!mRequestOptions.WriteFile.IsNull()) {
        writeToFile(mRequestOptions.WriteFile.Get());
    }
    if (!mRequestOptions.ReadFile.IsNull()) {
        readFromFile(mRequestOptions.ReadFile.Get());
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
        mLogger.Debug() << "Add header: " << header;
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
    if (!mRequestOptions.BasicAuthUsername.empty()) {
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
