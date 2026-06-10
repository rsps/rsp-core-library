/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen, Steffen Brummer
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <map>
#include <string>
#include <rsp/network/ResponseParser.h>
#include <rsp/network/MultipartBody.h>
#include <rsp/posix/FileIO.h>
#include "CurlHttpRequest.h"
#include "CurlSession.h"
#include "Exceptions.h"

//#define LOG_OUTPUT 1
#ifdef LOG_OUTPUT
    #include <rsp/logging/BufferToStream.h>
#endif

using namespace rsp::logging;
using namespace rsp::network;
using namespace rsp::utils;

namespace rsp::network::curl
{

CurlHttpRequest::CurlHttpRequest()
    : mResponse(*this)
{
}

void CurlHttpRequest::readFromStream(const HttpBody_t& arBody)
{
    if (!arBody) {
        return;
    }
    if (arBody->GetStreamSize() == 0) {
        return;
    }
    if (dynamic_cast<MultipartBody*>(arBody.get())) {
        mRequestOptions.Headers.try_emplace("Content-Type", dynamic_cast<MultipartBody&>(*arBody).GetBoundary().GetContentTypeHeader()); // Add header with boundary
    }

    setCurlOption(CURLOPT_UPLOAD, 1L);
    setCurlOption(CURLOPT_READFUNCTION, streamReadFunction);
    setCurlOption(CURLOPT_READDATA, arBody.get());
    setCurlOption(CURLOPT_INFILESIZE_LARGE, arBody->GetStreamSize());
}

size_t CurlHttpRequest::writeFunction(void *apPtr, const size_t aSize, const size_t aMemberCount, const CurlHttpResponse *apResponse)
{
    apResponse->mpBody->Write({ static_cast<std::byte*>(apPtr), aSize * aMemberCount });
    return aSize * aMemberCount;
}

size_t CurlHttpRequest::streamReadFunction(void *apPtr, const size_t aSize, const size_t aMemberCount, const IStreamDataProvider *apDataProvider)
{
    size_t written = apDataProvider->Read(std::span(static_cast<std::byte*>(apPtr), aSize * aMemberCount));
#ifdef LOG_OUTPUT
    auto o = rsp::logging::LoggerInterface::GetDefault()->Info();
    o << "Request chunk (" << written << ") " << BufferToStream(static_cast<char*>(ptr), written, true);
#endif
    return written;
}

size_t CurlHttpRequest::headerFunction(const char *apData, const size_t aSize, const size_t aMemberCount, CurlHttpResponse *apResponse)
{
    apResponse->mHeaderData += std::string(apData, aSize * aMemberCount);
    if (apResponse->mHeaderData.ends_with(cHeaderEnd)) {
        ResponseParser parser(*apResponse);
        parser.ParseNewData({ reinterpret_cast<const std::byte*>(apResponse->mHeaderData.data()), apResponse->mHeaderData.size() });
    }

    return (aSize * aMemberCount);
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

IHttpRequest& CurlHttpRequest::SetBody(HttpBody_t apBody)
{
    mRequestOptions.RequestBody = apBody;
    return *this;
}

const IStreamDataProvider& CurlHttpRequest::GetBody() const
{
    return *mRequestOptions.RequestBody;
}

IHttpRequest& CurlHttpRequest::SetResponseBody(HttpBody_t apBody)
{
    mRequestOptions.ResponseBody = apBody;
    return *this;
}

const IStreamDataProvider& CurlHttpRequest::GetResponseBody() const
{
    return *mRequestOptions.ResponseBody;
}

IHttpResponse& CurlHttpRequest::execute()
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
    mResponse.Clear();
}

void CurlHttpRequest::requestDone()
{
    mLogger.Debug() << "Request to " << mRequestOptions.BaseUrl << mRequestOptions.Uri << " is finished with code " << std::to_underlying(mResponse.GetStatusCode());
    EasyCurl::requestDone();
}

std::uintptr_t CurlHttpRequest::GetHandle() const
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
                readFromStream(mRequestOptions.RequestBody);
            }
            break;

        case HttpRequestType::HEAD:
            setCurlOption(CURLOPT_NOBODY, 1L);
            break;

        case HttpRequestType::PATCH:
            setCurlOption(CURLOPT_CUSTOMREQUEST, "PATCH");
            readFromStream(mRequestOptions.RequestBody);
            break;

        case HttpRequestType::PUT:
            // set Curl Option(CURLOPT_PUT, 1L); // Seems to put files only
            setCurlOption(CURLOPT_CUSTOMREQUEST, "PUT");
            readFromStream(mRequestOptions.RequestBody);
            break;

        case HttpRequestType::DELETE:
            setCurlOption(CURLOPT_CUSTOMREQUEST, "DELETE");
            break;

        default:
            break;
    }

    setCurlOption(CURLOPT_VERBOSE, mRequestOptions.Verbose ? 1L : 0L);

    //Redirect configuration
    setCurlOption(CURLOPT_FOLLOWLOCATION, _followRedirects);
    setCurlOption(CURLOPT_MAXREDIRS, _maxRedirects);

    //Progress and keep-alive configuration
    setCurlOption(CURLOPT_NOPROGRESS, 1L);
    setCurlOption(CURLOPT_TCP_KEEPALIVE, 1L);

    setCurlOption(CURLOPT_CONNECTTIMEOUT, mRequestOptions.ConnectionTimeout);
    setCurlOption(CURLOPT_SERVER_RESPONSE_TIMEOUT, mRequestOptions.ResponseTimeout);

//    setCurlOption(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0)
    setCurlOption(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);

//    CURLMOPT_PIPELINING to CURLPIPE_MULTIPLEX
//    CURLOPT_PIPEWAIT

    if (!mRequestOptions.CertCaPath.empty()) {
        setCurlOption(CURLOPT_CAINFO, mRequestOptions.CertCaPath.c_str());
    }
    setCurlOption(CURLOPT_SSL_VERIFYPEER, mRequestOptions.mVerifyPeer ? 1L : 0L);

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
    for (auto const& [first, second] : mRequestOptions.Headers) {
        std::string header = std::format("{}: {}", first, second);
        mLogger.Debug() << "Add header: " << header;
        auto *temp = curl_slist_append(mpHeaders, header.c_str());
        if (temp == nullptr) {
            curl_slist_free_all(mpHeaders);
            mpHeaders = nullptr;
            THROW_WITH_BACKTRACE1(ECurlError, "curl_slist_append failed.");
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
 * \return IHttpRequest unique pointer
 */
std::unique_ptr<IHttpRequest> rsp::network::IHttpRequest::Create()
{
    return std::make_unique<rsp::network::curl::CurlHttpRequest>();
}
