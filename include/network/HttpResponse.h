/**
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
    HttpResponse(IHttpRequest &arRequest)
        : mrRequest(arRequest)
    {
    }
    std::map<std::string, std::string>& GetHeaders() override
    {
        return mHeaders;
    }
    IHttpResponse& SetHeaders(std::map<std::string, std::string> &arHeaders) override
    {
        mHeaders = arHeaders;
        return *this;
    }
    IHttpResponse& AddHeader(std::string aKey, std::string aValue) override
    {
        mHeaders[aKey] = aValue;
        return *this;
    }

    int GetStatusCode() override
    {
        return mStatusCode;
    }
    IHttpResponse& SetStatusCode(int aCode) override
    {
        mStatusCode = aCode;
        return *this;
    }
    IHttpRequest& GetRequest() override
    {
        return mrRequest;
    }

    std::string& GetBody() override
    {
        return mBody;
    }

    IHttpResponse& PickBody(std::string &arBody)
    {
        mBody = std::move(arBody);
        return *this;
    }

    IHttpResponse& SetBody(const std::string &arBody)
    {
        mBody = arBody;
        return *this;
    }

protected:
    IHttpRequest &mrRequest;
    int mStatusCode = 0;
    std::map<std::string, std::string> mHeaders { };
    std::string mBody { };
};

}// namespace rsp::network

#endif // HTTPRESPONSE_H
