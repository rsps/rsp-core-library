/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 * \author:      Steffen Brummer
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDE_NETWORK_IHTTPSESSION_H
#define INCLUDE_NETWORK_IHTTPSESSION_H

#include <network/IHttpRequest.h>
#include <network/HttpRequestOptions.h>

namespace rsp::network {

class IHttpSession
{
public:
    typedef std::function<void(IHttpResponse&)> ResponseCallback_t;

    virtual ~IHttpSession() {}

    virtual void ProcessRequests() = 0;

    virtual IHttpSession& SetDefaultOptions(const HttpRequestOptions &arOptions) = 0;

    virtual IHttpRequest& Request(HttpRequestType aType, std::string_view aUri, ResponseCallback_t aCallback) = 0;

    IHttpRequest& Get(std::string_view aUri, ResponseCallback_t aCallback)   { return Request(HttpRequestType::GET, aUri, aCallback); }
    IHttpRequest& Post(std::string_view aUri, ResponseCallback_t aCallback)  { return Request(HttpRequestType::POST, aUri, aCallback); }
    IHttpRequest& Put(std::string_view aUri, ResponseCallback_t aCallback)   { return Request(HttpRequestType::PUT, aUri, aCallback); }
    IHttpRequest& Head(std::string_view aUri, ResponseCallback_t aCallback)  { return Request(HttpRequestType::HEAD, aUri, aCallback); }
    IHttpRequest& Patch(std::string_view aUri, ResponseCallback_t aCallback) { return Request(HttpRequestType::PATCH, aUri, aCallback); }
    IHttpRequest& Delete(std::string_view aUri, ResponseCallback_t aCallback){ return Request(HttpRequestType::DELETE, aUri, aCallback); }
};

} // namespace rsp::network

#endif
