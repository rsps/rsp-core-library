/**
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
#include <stdexcept>
#include <utility>
#include "EasyCurl.h"

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
    void Execute(std::function<void(rsp::network::IHttpResponse&)> callback) override;
    IHttpResponse& Execute() override;
    HttpRequestOptions& GetOptions() override;
    IHttpRequest& SetHeaders(std::map<std::string,std::string> &headers) override;
    IHttpRequest& SetOptions(const HttpRequestOptions opt) override;
    IHttpRequest& SetBody(std::string const &body) override;

protected:
    HttpResponse mResponse;
    HttpRequestOptions mRequestOptions;

private:

    static size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string *data);
    static size_t headerFunction(void *data, size_t size, size_t nmemb, void *userdata);
    void checkRequestOptions(HttpRequestOptions aOpts);
};
}

#endif
