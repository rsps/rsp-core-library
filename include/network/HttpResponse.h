/*!
 * \copyright        Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license          Mozilla Public License 2.0
 * \author:          Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <network/IHttpRequest.h>
#include <network/IHttpResponse.h>

namespace rsp::network {
/**
 * \class HttpResponse
 *
 * \brief This is the output when making a HttpRequest and implements the IHttpResponse interface.
 *
 */
class HttpResponse: public IHttpResponse
{
public:
    explicit HttpResponse(IHttpRequest &arRequest)
        : mrRequest(arRequest)
    {
    }

    [[nodiscard]] const std::map<std::string, std::string>& GetHeaders() const override
    {
        return mHeaders;
    }

    [[nodiscard]] const std::string& GetHeader(const std::string &arName) const override;

    [[nodiscard]] int GetStatusCode() const override
    {
        return mStatusCode;
    }

    [[nodiscard]] const IHttpRequest& GetRequest() const override
    {
        return mrRequest;
    }

    [[nodiscard]] const std::string& GetBody() const override
    {
        return mBody;
    }

protected:
    IHttpRequest &mrRequest;
    int mStatusCode = 0;
    std::map<std::string, std::string> mHeaders { };
    std::string mBody { };
};

}// namespace rsp::network

#endif // HTTPRESPONSE_H
