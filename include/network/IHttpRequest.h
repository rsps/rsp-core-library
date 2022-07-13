/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef I_HTTPREQUEST_H
#define I_HTTPREQUEST_H

#include <memory>
#include <functional>
#include <network/HttpRequestOptions.h>
#include <network/IHttpResponse.h>

namespace rsp::network {

class IHttpResponse;

class IHttpRequest
{
public:
    virtual ~IHttpRequest() {}

    virtual const HttpRequestOptions& GetOptions() const = 0;
    virtual IHttpRequest & SetOptions(const HttpRequestOptions &arOptions) = 0;

    //virtual std::string const GetBody() = 0;
    virtual IHttpRequest & SetBody(const std::string &arBody) = 0;

    virtual IHttpResponse& Execute() = 0;

    virtual std::uintptr_t GetHandle() = 0;
};

std::ostream& operator<<(std::ostream &o, const IHttpRequest& arReq);

}

#endif
