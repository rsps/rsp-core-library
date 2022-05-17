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



class IHttpRequest;

class IHttpResponse
{
public:
    virtual int GetStatusCode() const = 0;

    virtual const std::map<std::string, std::string>& GetHeaders() const = 0;
    virtual const IHttpResponse& SetHeaders(std::map<std::string, std::string> & headers) = 0;
    virtual const std::map<std::string, std::string>& AddHeader(std::string key, std::string value) = 0;
    
    virtual const IHttpRequest& GetRequest() const = 0;
    virtual const std::string& GetBody() const = 0;

    virtual ~IHttpResponse() { }
};

inline std::ostream& operator<<(std::ostream &aStream, IHttpResponse & resp) 
{
    for (const auto &[k, v] : resp.GetHeaders() )
        aStream << ("[" + k + ", " + v + "]\n");
    return aStream << "\n" << " StatusCode = " << std::to_string(resp.GetStatusCode()) << "\n"; 
}
}

#endif
