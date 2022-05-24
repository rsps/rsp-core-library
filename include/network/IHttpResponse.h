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

#ifndef I_HTTPRESPONSE_H
#define I_HTTPRESPONSE_H

#include <map>
#include <string>
#include <network/IHttpRequest.h>

namespace rsp::network::http {

//Forward declerations
class IHttpRequest;

class IHttpResponse
{
public:
    //Headers
    virtual std::map<std::string, std::string> & GetHeaders() = 0;
    virtual IHttpResponse & SetHeaders(std::map<std::string, std::string> & headers) = 0;
    virtual IHttpResponse & AddHeader(std::string key, std::string value) = 0;    
    
    //HTTP response code
    virtual int GetStatusCode() = 0;
    virtual IHttpResponse & SetStatusCode(int & arCode) = 0;
    
    //Origin request
    virtual IHttpRequest & GetRequest() = 0;
    
    //HTTP response body
    virtual std::string & GetBody() = 0;
    virtual IHttpResponse & SetBody(std::string & arBody) = 0;

    virtual ~IHttpResponse() { }
};

//Operator overloads
inline std::ostream& operator<<(std::ostream & arStream, IHttpResponse & resp) 
{
    for (const auto &[k, v] : resp.GetHeaders() )
        arStream << ("[" + k + ", " + v + "]\n");
    return arStream << "\n" << " StatusCode = " << std::to_string(resp.GetStatusCode()) << "\n"; 
}
}

#endif
