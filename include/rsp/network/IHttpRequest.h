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
#include <rsp/network/HttpRequestOptions.h>
#include <rsp/network/IHttpResponse.h>
#include <rsp/posix/FileIO.h>
#include <type_traits>

namespace rsp::network {

class IHttpResponse;

template <typename T>
concept OfTypeIStreamDataProvider = std::is_base_of<IStreamDataProvider, T>::value;

/**
 * \class IHttpRequest
 * \brief Interface for a HTTP Request object.
 */
class IHttpRequest
{
public:
    /**
     * \brief Factory method for creating IHttpRequest objects.
     * \return Pointer to IHttpRequest object.
     */
    static std::unique_ptr<IHttpRequest> Create();

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
     * \param apBody Shared pointer to interface of IStreamDataProvider
     * \return self
     */
    virtual IHttpRequest& SetBody(HttpBody_t apBody) = 0;

    template <OfTypeIStreamDataProvider C, typename... aArgs>
    IHttpRequest& MakeBody(aArgs&&... args)
    {
        return SetBody(std::make_shared<C>(std::forward<aArgs>(args)...));
    }

    /**
     * \brief Get the body content of this request
     * \return Reference to body stream
     */
    [[nodiscard]] virtual const IStreamDataProvider& GetBody() const = 0;

    /**
     * \brief Set the body container for the response.
     *
     * \param apBody Shared pointer to interface of IStreamDataProvider
     * \return self
     */
    virtual IHttpRequest& SetResponseBody(HttpBody_t apBody) = 0;

    template <OfTypeIStreamDataProvider C, typename... aArgs>
    IHttpRequest& MakeResponseBody(aArgs&&... args)
    {
        return SetResponseBody(std::make_shared<C>(std::forward<aArgs>(args)...));
    }

    /**
     * \brief Get the response body container
     * \return Reference to body stream
     */
    [[nodiscard]] virtual const IStreamDataProvider& GetResponseBody() const = 0;

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
    [[nodiscard]] virtual std::uintptr_t GetHandle() const = 0;
};

/**
 * \fn std::ostream operator <<&(std::ostream&, const IHttpRequest&)
 * \brief Output streaming operator for the HttpRequest interface. Useful during debugging.
 *
 * \param o Output stream to write to.
 * \param arReq The HTTP request to dump to the stream.
 * \return output stream
 */
inline std::ostream& operator<<(std::ostream &o, const IHttpRequest& arReq)
{
    o << arReq.GetOptions();
    return o;
}


} // namespace rsp::network

#endif // RSP_CORE_LIB_NETWORK_I_HTTP_REQUEST_H
