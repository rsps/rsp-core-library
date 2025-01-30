/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_NETWORK_URL_PARSER_H
#define RSP_CORE_LIB_INCLUDE_NETWORK_URL_PARSER_H

#include <cstdint>
#include <exceptions/CoreException.h>
#include <ostream>
#include <string_view>

namespace rsp::network {

class UrlParserException : public rsp::exceptions::CoreException
{
public:
    using rsp::exceptions::CoreException::CoreException;
};

class EInvalidUrl : public UrlParserException
{
public:
    using UrlParserException::UrlParserException;
};

/**
 * \brief Class that decodes a URI/URL according to RFC-3986
 */
class UrlParser
{
public:
    enum class Scheme {
        Unknown = -1,
        Http = 80,
        Https = 443
    };
    using Port = uint16_t;

    explicit UrlParser(std::string_view aUrl);

    [[nodiscard]] bool RequiresTLS() const;

    [[nodiscard]] Scheme GetScheme() const { return mScheme; }
    [[nodiscard]] std::string_view GetHost() const { return mHost; }
    [[nodiscard]] Port GetPort() const { return mPort; }
    [[nodiscard]] std::string_view GetPath() const { return mPath; }
    [[nodiscard]] std::string_view GetQuery() const { return mQuery; }
    [[nodiscard]] std::string_view GetFragment() const { return mFragment; }
    [[nodiscard]] std::string_view GetUserInfo() const { return mUserInfo; }

protected:
    /* URI format, from RFC-3986:
     *    foo://example.com:8042/over/there?name=ferret#nose
     *    \_/   \______________/\_________/ \_________/ \__/
     *     |           |            |            |        |
     *  scheme     authority       path        query   fragment
     */
    Scheme mScheme = Scheme::Unknown; // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    std::string_view mHost{}; // authority = [ userinfo "@" ] host [ ":" port ]
    std::string_view mUserInfo{};
    Port mPort = 0;
    std::string_view mPath{};
    std::string_view mQuery{};
    std::string_view mFragment{};

    void parseScheme(size_t &arPosition, std::string_view aUrl);
    void parseHost(size_t &arPosition, std::string_view aUrl);
    void parseHostAndPort(std::string_view aDomainName);
    void parsePath(size_t &arPosition, std::string_view aUrl);
    void parseQuery(size_t &arPosition, std::string_view aUrl);
    void parseFragment(size_t &arPosition, std::string_view aUrl);
};

std::ostream& operator<<(std::ostream &o, const UrlParser& arParser);

} // rsp::network

#endif //RSP_CORE_LIB_INCLUDE_NETWORK_URL_PARSER_H
