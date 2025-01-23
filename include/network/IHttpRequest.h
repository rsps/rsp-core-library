/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RSP_CORE_LIB_NETWORK_I_HTTP_REQUEST_H
#define RSP_CORE_LIB_NETWORK_I_HTTP_REQUEST_H

#include <memory>
#include <ostream>
#include <functional>
#include <network/HttpRequestOptions.h>
#include <network/IHttpResponse.h>
#include <posix/FileIO.h>

namespace rsp::network {

class IHttpResponse;

/**
 * \class IHttpRequest
 * \brief Interface for a HTTP Request object.
 */
class IHttpRequest
{
public:
    virtual ~IHttpRequest() = default;

    /**
     * \fn const HttpRequestOptions GetOptions&()const =0
     * \brief Get a const reference to the internal options object.
     *
     * \return Reference to options
     */
    [[nodiscard]] virtual const HttpRequestOptions& GetOptions() const = 0;

    /**
     * \fn IHttpRequest SetOptions&(const HttpRequestOptions&)=0
     * \brief Set all options on this HTTP request. This also includes request headers.
     *
     * \param arOptions
     * \return self
     */
    virtual IHttpRequest& SetOptions(const HttpRequestOptions &arOptions) = 0;

    /**
     * \brief Set the body content on the request.
     *
     * \param apBody Shared pointer to interface of IHttpBodyStream
     * \return self
     */
    virtual IHttpRequest& SetBody(std::shared_ptr<IHttpBodyStream> apBody) = 0;

    /**
     * \brief Get the body content of this request
     * \return Reference to body stream
     */
    [[nodiscard]] virtual const IHttpBodyStream& GetBody() const = 0;

    /**
     * \fn IHttpForm AddField&(std::string_view, std::string_view)
     * \brief Add a form field as key/value pair
     *
     * \param aFieldName Name of the field
     * \param aValue Value of the field
     * \return self
     */
    virtual IHttpRequest& AddField(const std::string &arFieldName, const std::string &arValue) = 0;

    /**
     * \fn IHttpForm AddFile&(std::string_view, rsp::posix::FileIO&)
     * \brief Add a file field to the form
     *
     * \param aFieldName Name of the field
     * \param arFile File value of the field
     * \return self
     */
    virtual IHttpRequest& AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile) = 0;

    /**
     * \fn IHttpResponse Execute&()=0
     * \brief Execute the request. This returns a response or throws on transport errors.
     *
     * \return IHttpResponse object reference.
     */
    virtual IHttpResponse& Execute() = 0;

    /**
     * \fn std::uintptr_t GetHandle()=0
     * \brief Get a handle for the underlying connection, be it socket or library identifier the uintptr_t should be able to hold it..
     *
     * \return Low level handle stored in generic uintptr_t type.
     */
    virtual std::uintptr_t GetHandle() = 0;
};

/**
 * \fn std::ostream operator <<&(std::ostream&, const IHttpRequest&)
 * \brief Output streaming operator for the HttpRequest interface. Useful during debugging.
 *
 * \param o Output stream to write to.
 * \param arReq The HTTP request to dump to the stream.
 * \return output stream
 */
std::ostream& operator<<(std::ostream &o, const IHttpRequest& arReq);

} // namespace rsp::network

#endif // RSP_CORE_LIB_NETWORK_I_HTTP_REQUEST_H