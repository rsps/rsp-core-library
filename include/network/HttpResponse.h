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

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <network/IHttpRequest.h>
#include <network/IHttpResponse.h>

namespace rsp::network::http {

class HttpResponse: public IHttpResponse
{
public:
    HttpResponse(IHttpRequest &arRequest)
        : mrRequest(arRequest)
    {
    }

    int GetStatusCode() const override
    {
        return mStatusCode;
    }

    HttpResponse & SetStatusCode(int const & code) { this->mStatusCode = code; return *this; }
    const IHttpResponse & SetHeaders(std::map<std::string, std::string>  & headers) override { this->mHeaders = headers; return *this; }
    HttpResponse & SetBody(std::string const & body) { this->mBody = body; return *this; }

    const std::map<std::string, std::string>& AddHeader(std::string key, std::string value) override {
        this->mHeaders[key] = value;
        return this->mHeaders;
    }


    const std::map<std::string, std::string>& GetHeaders() const override
    {
        return mHeaders;
    }

    const IHttpRequest& GetRequest() const override
    {
        return mrRequest;
    }

    const std::string& GetBody() const override
    {
        return mBody;
    }

    

    
protected:
    IHttpRequest &mrRequest;
    int mStatusCode = 0;
    std::map<std::string, std::string> mHeaders { };
    std::string mBody { };
};
}

#endif
