/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <rsp/utils/EnumReflection.h>
#include <rsp/network/parser-helpers.h>
#include <rsp/network/UrlParser.h>

using namespace std::string_view_literals;

namespace rsp::network {

std::ostream& operator<<(std::ostream& o, const UrlParser& arParser)
{
    o
        << "Scheme:    " << utils::EnumName(arParser.GetScheme()) << "\n"
        << "TLS:       " << arParser.RequiresTLS() << "\n"
        << "Host:      " << arParser.GetHost() << "\n"
        << "Port:      " << arParser.GetPort() << "\n"
        << "Path:      " << arParser.GetPath() << "\n"
        << "Query:     " << arParser.GetQuery() << "\n"
        << "Fragment:  " << arParser.GetFragment() << "\n"
        << "User Info: " << arParser.GetUserInfo();
    return o;
}

/**
 * \brief Determine if the given scheme requires TLS encryption
 * \param Scheme
 * \return
 */
[[nodiscard]] constexpr bool is_tls_encrypted(UrlParser::Scheme aScheme) noexcept
{
    constexpr  UrlParser::Scheme map[] = { UrlParser::Scheme::Https };
    return std::ranges::contains(map, aScheme);
}

/**
 * \brief Get the scheme that corresponds to the given string
 * \param aSchemeString
 * \return Scheme
 */
[[nodiscard]] constexpr UrlParser::Scheme get_scheme_from_string(std::string_view const aSchemeString) noexcept
{
    if (equal_ascii_case_insensitive(aSchemeString, "http")) {
        return UrlParser::Scheme::Http;
    }
    else if (equal_ascii_case_insensitive(aSchemeString, "https")) {
        return UrlParser::Scheme::Https;
    }
    return UrlParser::Scheme::Unknown;
}

bool UrlParser::RequiresTLS() const
{
    return is_tls_encrypted(mScheme);
}

UrlParser::UrlParser(std::string_view aUrl)
{
    // Find the start position of the protocol.
    auto start_position = aUrl.find_first_not_of(cWhitespaceCharacters);
    if (start_position == std::string_view::npos) {
        THROW_WITH_BACKTRACE1(EInvalidUrl, "Url must not be empty");
    }

    parseScheme(start_position, aUrl);
    parseHost(start_position, aUrl);
    parsePath(start_position, aUrl);
    parseQuery(start_position, aUrl);
    parseFragment(start_position, aUrl);
}

void UrlParser::parseScheme(size_t& arPosition, std::string_view aUrl)
{
    // Find the end position of the protocol.
    auto const position = aUrl.find(cSchemeSuffix, arPosition);
    if (position == std::string_view::npos) {
        THROW_WITH_BACKTRACE1(EInvalidUrl, "Url must start with a scheme (http/https)");
    }
    mScheme = get_scheme_from_string(aUrl.substr(arPosition, position - arPosition));
    if (mScheme == Scheme::Unknown) {
        THROW_WITH_BACKTRACE1(EInvalidUrl, std::string("The Url scheme '") + std::string(aUrl.substr(arPosition, position - arPosition)) + std::string("' is not supported"));
    }
    mPort = Port(mScheme); // Default port for the current scheme.

    arPosition = position + cSchemeSuffix.length();
}

void UrlParser::parseHost(size_t& arPosition, std::string_view aUrl)
{
    // Find the end position of the domain name and start of the path.
    auto slash_position = aUrl.find_first_of("/#?", arPosition);
    if (slash_position == std::string_view::npos) {
        slash_position = aUrl.length();
    }

    parseHostAndPort(aUrl.substr(arPosition, slash_position - arPosition));
    if (auto user_position = mHost.find('@'); user_position != std::string_view::npos) {
        mUserInfo = mHost.substr(0, user_position);
        if (mUserInfo.empty()) {
            THROW_WITH_BACKTRACE1(EInvalidUrl, "Url user MUST have content");
        }
        mHost = mHost.substr(user_position + 1);
    }

    if (mHost.empty()) {
        THROW_WITH_BACKTRACE1(EInvalidUrl, "Url host is not valid");
    }

    arPosition = slash_position;
}

void UrlParser::parseHostAndPort(std::string_view aDomainName)
{
    auto colon_pos = aDomainName.find_last_of(":]"); // Find last colon, NOT belonging to ipv6 address.
    if (colon_pos == std::string_view::npos || aDomainName.at(colon_pos) != ':') {
        mHost = aDomainName;
        return;
    }
    mHost = aDomainName.substr(0, colon_pos);

    auto number = string_to_integral<int>(aDomainName.substr(colon_pos + 1));
    if (!number || *number < 0 || *number > std::numeric_limits<Port>::max()) {
        THROW_WITH_BACKTRACE1(EInvalidUrl, "Url does not have a valid port number");
    }
    mPort = Port(*number);
}

void UrlParser::parsePath(size_t& arPosition, std::string_view aUrl)
{
    if (arPosition >= aUrl.length() || aUrl.at(arPosition) != '/') {
        mPath = "/"sv;
        return;
    }
    auto end_position = aUrl.find_first_of("?#", arPosition);
    if (end_position == std::string_view::npos) {
        end_position = aUrl.length();
    }
    mPath = aUrl.substr(arPosition, end_position - arPosition);
    arPosition = end_position;
}

void UrlParser::parseQuery(size_t& arPosition, std::string_view aUrl)
{
    if (arPosition >= aUrl.length() || aUrl.at(arPosition) != '?') {
        return;
    }
    arPosition += 1;
    auto end_position = aUrl.find('#', arPosition);
    if (end_position == std::string_view::npos) {
        end_position = aUrl.length();
    }
    mQuery = aUrl.substr(arPosition, end_position - arPosition);
    arPosition = end_position;
}

void UrlParser::parseFragment(size_t& arPosition, std::string_view aUrl)
{
    if (arPosition >= aUrl.length() || aUrl.at(arPosition) != '#') {
        return;
    }
    arPosition += 1;
    auto end_position = aUrl.find_last_not_of(cWhitespaceCharacters) + 1;
    mFragment = aUrl.substr(arPosition, end_position - arPosition); // Rest of line is fragment
    arPosition = end_position;
}

} // rsp::network
