/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_HTTP_TEXT_H
#define RSP_CORE_LIB_HTTP_TEXT_H

#include <exceptions/CoreException.h>
#include <span>
#include <string_view>

namespace rsp::network {

class EHttpParseError: public rsp::exceptions::CoreException
{
public:
    explicit EHttpParseError(const char *aMsg = "Not Implemented")
          : CoreException(aMsg)
    {
    }
    explicit EHttpParseError(const std::string &arMsg)
          : CoreException(arMsg)
    {
    }
};

class HttpText
{
public:
    static constexpr size_t npos = std::string_view::npos;

    explicit HttpText(std::string_view aSource);

    HttpText& CR(); // Carriage return
    HttpText& CRLF(); // Carriage Return + Line Feed
    HttpText& LF(); // Line Feed
    HttpText& OWS(); // Optional White Space
    HttpText& RWS(size_t aCount = 1); // Required White Space
    std::string_view Alpha(size_t aSize = npos);
    int Digit(size_t aCount);
    int HexDigit(size_t aCount);
    std::span<const std::byte> Octets(size_t aSize = npos);
    std::string_view Key();
    std::string_view Value();
    HttpText& Rewind();

protected:
    std::string_view mSource;
    size_t mCursor = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_HTTP_TEXT_H
