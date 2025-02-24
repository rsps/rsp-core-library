/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RSP_CORE_LIB_NETWORK_I_HTTP_RESPONSE_H
#define RSP_CORE_LIB_NETWORK_I_HTTP_RESPONSE_H

#include <exceptions/CoreException.h>
#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include "parser-helpers.h"
#include <string>
#include <utils/string_view_ci.h>
#include "IHttpRequest.h"
#include "IStreamDataProvider.h"
#include "StatusCodes.h"
#include "StatusLine.h"

namespace rsp::network {


class EHeaderNotFound : public exceptions::CoreException
{
public:
    using rsp::exceptions::CoreException::CoreException;
};

//Forward declarations
class IHttpRequest;

/**
 * \class IHttpResponse
 * \brief Interface for a HTTP response object
 */
class IHttpResponse
{
public:
    using HeaderList = utils::string_view_map_ci;

    virtual ~IHttpResponse() = default;

    /**
     * \brief Get the received status line as a decoded object
     * \return StatusLine
     */
    [[nodiscard]] virtual const StatusLine& GetStatusLine() const = 0;

    /**
     * \brief Get a const reference to the response headers.
     *
     * \return Reference to headers.
     */
    [[nodiscard]] virtual const HeaderList& GetHeaders() const = 0;

    /**
     * \brief Get a const reference to the specific header value.
     * \param arName
     * \return Reference to header value
     */
    [[nodiscard]] virtual std::string_view GetHeader(std::string_view aName) const = 0;

    /**
     * \fn int GetStatusCode()const =0
     * \brief Get the status code of the response
     *
     * \return StatusCodes status code returned from server
     */
    [[nodiscard]] virtual StatusCodes GetStatusCode() const = 0;

    /**
     * \brief Get the length of the body part.
     * \return size_t
     */
    [[nodiscard]] virtual size_t GetContentLength() const = 0;

    /**
     * \fn const IHttpRequest GetRequest&()const =0
     * \brief Get the request object causing this response.
     *
     * \return Reference to the request object.
     */
    [[nodiscard]] virtual const IHttpRequest& GetRequest() const = 0;

    /**
     * \fn const std::string GetBody&()const =0
     * \brief Get the body of the response.
     *
     * \return String with body content
     */
    [[nodiscard]] virtual IStreamDataProvider& GetBody() const = 0;

    /**
     * \brief Create the Body object. This is to be called when header section
     *        is completed, and remaining data should go to the body.
     * \return self
     */
    virtual IHttpResponse& MakeBody() = 0;

    /**
     * \brief Clear the response so it can be reused.
     * \return self
     */
    virtual IHttpResponse& Clear() = 0;
};

std::ostream& operator<<(std::ostream &o, const IHttpResponse &arResponse);

} // rsp::network

#endif // RSP_CORE_LIB_NETWORK_I_HTTP_RESPONSE_H