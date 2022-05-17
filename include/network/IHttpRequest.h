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

#ifndef I_HTTPREQUEST_H
#define I_HTTPREQUEST_H

#include <memory>
#include <functional>
#include <network/HttpRequestOptions.h>
#include <network/IHttpResponse.h>

namespace rsp::network::http {

class IHttpResponse;

class IHttpRequest
{
public:
    virtual HttpRequestOptions& GetOptions() = 0;
    virtual IHttpRequest & SetOptions(const HttpRequestOptions opt) = 0;
    
    //virtual std::map<std::string, std::string> GetHeaders() = 0;
    virtual IHttpRequest & SetHeaders(std::map<std::string, std::string>  & headers) = 0;

    //virtual std::string const GetBody() = 0;
    virtual IHttpRequest & SetBody(std::string const & body) = 0;

    virtual IHttpResponse& Execute() = 0;
    virtual void Execute(std::function<void(IHttpResponse&)> callback) = 0;
    virtual ~IHttpRequest()
    {
    }
};

inline std::ostream& operator<<(std::ostream &aStream, IHttpRequest & resp) 
{
    return aStream << "Request print not defined yet.\n";
}

}

#endif
