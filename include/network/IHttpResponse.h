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
#include "IHttpRequest.h"
#include "IStreamDataProvider.h"
#include "StatusCodes.h"

namespace rsp::network {


class EHeaderNotFound : public exceptions::CoreException
{
public:
    explicit EHeaderNotFound(const std::string &arMsg)
        : CoreException(arMsg)
    {
    }
};

//Forward declarations
class IHttpRequest;

namespace detail
{

/**
 * \brief Trait class for case insensitive comparison
 * \see https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
 */
struct ci_char_traits : public std::char_traits<char>
{
    static bool eq(char c1, char c2) { return toupper(c1) == toupper(c2); }
    static bool ne(char c1, char c2) { return toupper(c1) != toupper(c2); }
    static bool lt(char c1, char c2) { return toupper(c1) <  toupper(c2); }
    static int compare(const char* s1, const char* s2, size_t n)
    {
        while( n-- != 0 ) {
            auto a = toupper(*s1);
            auto b = toupper(*s2);
            if( a < b ) return -1;
            if( a > b ) return 1;
            ++s1;
            ++s2;
        }
        return 0;
    }
    static const char* find(const char* s, int n, char a)
    {
        auto a_upper = toupper(a);
        while( n-- > 0 && toupper(*s) != a_upper ) {
            ++s;
        }
        return s;
    }
};

/**
 * \brief Case insensitive std::less comparator for map<string,*> types
 */
struct CaseInsensitiveComparator
{
    bool operator()(std::string_view a, std::string_view b) const noexcept
    {
        std::basic_string_view<std::string::value_type, ci_char_traits> ci_view1{ a.data(), a.size() };
        std::basic_string_view<std::string::value_type, ci_char_traits> ci_view2{ b.data(), b.size() };
        return ci_view1 < ci_view2;
    }
};

}   // namespace detail

/**
 * \class IHttpResponse
 * \brief Interface for a HTTP response object
 */
class IHttpResponse
{
public:
    using HeaderList = std::map<std::string_view, std::string_view, detail::CaseInsensitiveComparator>;

    virtual ~IHttpResponse() = default;

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