/**
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 * Created Date:  Tuesday, May 17th 2022, 8:49:44 am
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef I_HTTPRESPONSE_H
#define I_HTTPRESPONSE_H

#include <iostream>
#include <map>
#include <string>
#include <network/IHttpRequest.h>

namespace rsp::network {

//Forward declarations
class IHttpRequest;

class IHttpResponse
{
public:
    //Headers
    virtual std::map<std::string, std::string>& GetHeaders() = 0;
    virtual IHttpResponse& SetHeaders(std::map<std::string, std::string> &headers) = 0;
    virtual IHttpResponse& AddHeader(std::string key, std::string value) = 0;

    //HTTP response code
    virtual int GetStatusCode() = 0;
    virtual IHttpResponse& SetStatusCode(int aCode) = 0;

    //Origin request
    virtual IHttpRequest& GetRequest() = 0;

    //HTTP response body
    virtual std::string& GetBody() = 0;
    virtual IHttpResponse& SetBody(const std::string &arBody) = 0;

    virtual ~IHttpResponse()
    {
    }
};

//Operator overloads
std::ostream& operator<<(std::ostream &arStream, IHttpResponse &resp);

} // rsp::network

#endif
