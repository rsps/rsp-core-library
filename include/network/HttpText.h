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
#include <format>
#include <span>
#include <string_view>

namespace rsp::network {

class EHttpParseError: public rsp::exceptions::CoreException
{
public:
    using rsp::exceptions::CoreException::CoreException;

    explicit EHttpParseError(std::string_view aSource, size_t aCursor)
          : CoreException(std::format("HTTP Parse Error at position {} in {}", aCursor, aSource))
    {
    }
};

/**
 * \brief HTTP Parser helper, for traversing a block of text according to RFC-7230.
 */
class HttpText
{
public:
    static constexpr size_t npos = std::string_view::npos;

    /**
     * \brief Constructor that takes a string_view for traversal.
     * \param aSource
     */
    explicit HttpText(std::string_view aSource);

    /**
     * \brief Check if the current position points to a newline
     * \return True if the current position is a newline.
     */
    [[nodiscard]] bool IsNewLine() const;

    /**
     * \brief Check if cursor is at the end of the source string.
     * \return True if no more data is in string.
     */
    [[nodiscard]] bool Eof() const;
    [[nodiscard]] explicit operator bool() const { return !Eof(); }

    /**
     * \brief Traverse an expected carriage return, or fail.
     * \return self
     */
    HttpText& CR();

    /**
     * \brief Traverse an expected carriage return and a line feed, or fail
     * \return self
     */
    HttpText& CRLF();

    /**
     * \brief Traverse an expected line feed, or fail
     * \return self
     */
    HttpText& LF();

    /**
     * \brief Traverse an expected space, or fail
     * \return self
     */
    HttpText& SP();

    /**
     * \brief Traverse all Optional White Space characters from current position.
     * \return self
     */
    HttpText& OWS();

    /**
     * \brief Traverse the given amount of Required White Space characters, or fail.
     * \param aCount
     * \return self
     */
    HttpText& RWS(size_t aCount = 1); // Required White Space

    /**
     * \brief Get the string fulfilling alpha character rules from current position.
     * \param aSize
     * \return string_view
     */
    std::string_view Alpha(size_t aSize = npos);

    /**
     * \brief Get the string fulfilling VCHAR+HTB+SP character rules from current position.
     * \return string_view
     */
    std::string_view AsciiText();

    /**
     * \brief Get HTTP version string. (HTTP/x.x)
     * \return string_view
     */
    std::string_view HttpVersion();

    /**
     * \brief Parse the next aCount amount of characters as a decimal value.
     * \param aCount
     * \return int
     */
    int Digit(size_t aCount);

    /**
     * \brief PArse thr next aCount amount of characters as a hexadecimal value.
     * \param aCount
     * \return int
     */
    int HexDigit(size_t aCount);

    /**
     * \brief Get the next aSize amount of bytes from the current position.
     * \param aSize
     * \return span
     */
    std::span<const std::byte> Octets(size_t aSize = npos);

    /**
     * \brief Decode a field name from the current position
     * \return string_view
     */
    std::string_view FieldName();

    /**
     * \brief Decode a field value from the current position. Trim surrounding whitespace.
     * \return string_view
     */
    std::string_view FieldValue();

    /**
     * \brief Extract a single line from the current position.
     * \return New HttpText instance, with new cursor starting at 0.
     */
    [[nodiscard]] HttpText Line();

    /**
     * \brief Get the character value at the current cursor position, or fail.
     * \return char
     */
    [[nodiscard]] char Peek() const { return mSource.at(mCursor); }

    /**
     * \brief Reset the cursor to 0.
     * \return self
     */
    HttpText& Rewind();

    /**
     * \brief Get the entire string_view this object operates on.
     * \return string_view
     */
    [[nodiscard]] std::string_view Source() const { return mSource; }

protected:
    std::string_view mSource;
    size_t mCursor = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_HTTP_TEXT_H
