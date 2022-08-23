/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
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

/**
 * \class IHttpResponse
 * \brief Interface for a HTTP response object
 */
class IHttpResponse
{
public:
    /**
     * \brief Get a const reference to the response headers.
     *
     * \return Reference to headers.
     */
    virtual const std::map<std::string, std::string>& GetHeaders() const = 0;

    /**
     * \fn int GetStatusCode()const =0
     * \brief Get the status code of the response
     *
     * \return integer status code
     */
    virtual int GetStatusCode() const = 0;

    /**
     * \fn const IHttpRequest GetRequest&()const =0
     * \brief Get the request object causing this response.
     *
     * \return Reference to the request object.
     */
    virtual const IHttpRequest& GetRequest() const = 0;

    /**
     * \fn const std::string GetBody&()const =0
     * \brief Get the body of the response.
     *
     * \return String with body content
     */
    virtual const std::string& GetBody() const = 0;

    virtual ~IHttpResponse()
    {
    }
};

std::ostream& operator<<(std::ostream &o, const IHttpResponse &arResponse);

} // rsp::network

#endif
