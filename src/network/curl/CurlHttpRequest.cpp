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
        data->append(static_cast<char *>(ptr), size * nmemb);
        return size * nmemb;
    }

    size_t CurlHttpRequest::header_callback(void *data, size_t size, size_t nmemb, void *userdata)
    {

        HttpResponse *r;
        r = reinterpret_cast<HttpResponse *>(userdata);
        std::string header(reinterpret_cast<char *>(data), size * nmemb);
        std::cout << header << std::endl;
        size_t seperator = header.find_first_of(':');
        if (std::string::npos == seperator)
        {
            trim(header);
            if (0 == header.length())
            {
                return (size * nmemb); // blank line;
            }
            r->AddHeader(header, "present");
        }
        else
        {
            std::string key = header.substr(0, seperator);
            trim(key);
            std::string value = header.substr(seperator + 1);
            trim(value);
            r->AddHeader(key, value);
        }

        return (size * nmemb);
    }

    IHttpRequest &CurlHttpRequest::SetOptions(HttpRequestOptions opts)
    {
        this->mRequestOptions = opts;
        return *this;
    }

    IHttpRequest &CurlHttpRequest::SetHeaders(std::map<std::string, std::string> &headers)
    {
        this->mRequestOptions.Headers = headers;
        return *this;
    }

    IHttpRequest &CurlHttpRequest::SetBody(std::string const &body)
    {
        this->mRequestOptions.Body = body;
        return *this;
    }

    void CurlHttpRequest::Execute(std::function<void(IHttpResponse &)> callback)
    {
    }

    IHttpResponse &CurlHttpRequest::Execute()
    {
        auto curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, std::string(mRequestOptions.BaseUrl + mRequestOptions.Uri).c_str());
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

            std::string response_string;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &mResponse);

            curl_easy_perform(curl);
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            std::cout << response_string << "\n"
                      << response_code << "\n";

            curl_easy_cleanup(curl);
            curl_global_cleanup();
            curl = NULL;
            // mResponse.SetBody(response_string).SetStatusCode(response_code).SetHeaders(header_string);
        }

        return mResponse;
    }

    HttpRequestOptions &CurlHttpRequest::GetOptions()
    {
        return mRequestOptions;
    }

    void CurlHttpRequest::checkVersion()
    {
        curl_version_info_data *d = curl_version_info(CURLVERSION_NOW);
        if (d->version_num < minimumCurlVersion())
        {
            THROW_WITH_BACKTRACE1(ECurlVersion, "Wrong libcurl version");
        }
    }

} // namespace rsp::network::http::curl

IHttpRequest *rsp::network::http::HttpRequest::MakeRequest()
{
    return new rsp::network::http::curl::CurlHttpRequest();
}