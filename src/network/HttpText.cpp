/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <rsp/network/HttpText.h>
#include <rsp/network/parser-helpers.h>
#include <cctype>

using namespace std::string_view_literals;

namespace rsp::network {

constexpr auto cDelimiters = R"("(),/:;<=>?@[\]{})"sv;
constexpr auto cCRLF = "\r\n"sv;
constexpr auto cSpaceTab = " \t"sv;

#define PARSE_ERROR() THROW_WITH_BACKTRACE2(EHttpParseError, mSource, mCursor)


HttpText::HttpText(std::string_view aSource)
    : mSource(aSource)
{
}

HttpText& HttpText::CR()
{
    if (mSource.at(mCursor) != '\r') {
        PARSE_ERROR();
    }
    mCursor++;
    return *this;
}

HttpText& HttpText::CRLF()
{
    CR();
    LF();
    return *this;
}

HttpText& HttpText::LF()
{
    if (mSource.at(mCursor) != '\n') {
        PARSE_ERROR();
    }
    mCursor++;
    return *this;
}

HttpText& HttpText::SP()
{
    if (mSource.at(mCursor) != ' ') {
        PARSE_ERROR();
    }
    mCursor++;
    return *this;
}

HttpText& HttpText::OWS()
{
    mCursor = mSource.find_first_not_of(cSpaceTab, mCursor);
    return *this;
}

std::optional<std::pair<std::string_view, std::string_view>> HttpText::OChunkExt()
{
    if (mSource.at(mCursor) == ';') {
        mCursor++;
        auto end = mSource.find_first_of(";\r", mCursor);
        if (end == npos) {
            PARSE_ERROR();
        }
        std::string_view key{};
        std::string_view value{};
        auto pos = mSource.find('=', mCursor);
        if (pos != npos) {
            key = token(mSource.substr(mCursor, pos - mCursor));
            value = token(mSource.substr(pos + 1, end - pos - 1));
        }
        else {
            key = token(mSource.substr(mCursor, end));
        }
        mCursor = end;
        return {{key, value}};
    }
    return {};
}

HttpText& HttpText::RWS(size_t aCount)
{
    if (mSource.substr(mCursor, aCount).find_first_not_of(cSpaceTab) != npos) {
        PARSE_ERROR();
    }
    mCursor += aCount;
    return *this;
}

std::string_view HttpText::Alpha(size_t aSize)
{
    auto sub = mSource.substr(mCursor, aSize);
    for (auto c : sub) {
        if (!std::isalpha(c)) {
            PARSE_ERROR();
        }
    }
    mCursor += sub.size();
    return sub;
}

std::string_view HttpText::AsciiText()
{
    auto pos = mSource.find(cCRLF);
    if (pos == npos) {
        PARSE_ERROR();
    }
    auto sub = mSource.substr(mCursor, pos - mCursor);
    for (auto c : sub) {
        if (cDelimiters.contains(c)) {
            PARSE_ERROR();
        }
    }
    mCursor = pos + 2;
    return sub;
}

std::string_view HttpText::HttpVersion()
{
    auto pos = mSource.find("HTTP/");
    if (pos != 0) {
        PARSE_ERROR();
    }
    pos = mSource.find_first_of(' ');
    if (pos > 8) {
        PARSE_ERROR();
    }
    mCursor = pos + 1;
    return mSource.substr(0, pos);
}

size_t HttpText::Digit(size_t aCount)
{
    if (aCount == 0) {
        aCount = mSource.find_first_of(cWhitespaceCharacters, mCursor) - mCursor;
    }
    auto sub = mSource.substr(mCursor, aCount);
    for (auto c : sub) {
        if (!std::isdigit(c)) {
            PARSE_ERROR();
        }
    }
    mCursor += sub.size();
    auto result = string_to_integral<size_t>(sub);
    if (!result) {
        PARSE_ERROR();
    }
    return *result;
}

size_t HttpText::HexDigit(size_t aCount)
{
    if (aCount == 0) {
        aCount = mSource.find_first_of("; \t\r\n", mCursor) - mCursor;
    }
    auto sub = mSource.substr(mCursor, aCount);
    for (auto c : sub) {
        if (!std::isxdigit(c)) {
            PARSE_ERROR();
        }
    }
    mCursor += sub.size();
    auto result = string_to_integral<size_t>(sub, 16);
    if (!result) {
        PARSE_ERROR();
    }
    return *result;
}

std::span<const std::byte> HttpText::Octets(size_t aSize)
{
    auto sub = mSource.substr(mCursor, aSize);
    mCursor += sub.size();
    return { reinterpret_cast<const std::byte*>(sub.data()), sub.size() };
}

std::string_view HttpText::FieldName()
{
    auto colon_pos = mSource.find(':', mCursor);
    if (colon_pos == npos) {
        PARSE_ERROR();
    }

    auto sub = mSource.substr(mCursor, colon_pos - mCursor);
    auto result = token(sub);
    mCursor = colon_pos + 1;
    return result;
}

std::string_view HttpText::token(std::string_view aSub)
{
    for (auto c : aSub) {
        if (!std::isgraph(c)) {
            PARSE_ERROR();
        }
        if (cDelimiters.contains(c)) {
            PARSE_ERROR();
        }
    }
    return aSub;
}

std::string_view HttpText::FieldValue()
{
    OWS();
    auto end_pos = mSource.find(cCRLF, mCursor);
    if (end_pos == npos) {
        PARSE_ERROR();
    }
    auto sub = mSource.substr(mCursor, end_pos - mCursor);
    mCursor += sub.size() + 2;
    if (sub.at(0) == '"') { // DQUOTE rules
        end_pos = sub.find_last_of('"');
        if (end_pos == npos) {
            PARSE_ERROR();
        }
        return sub.substr(1, end_pos - 1);
    }

    end_pos = sub.find_last_not_of(cSpaceTab);
    if (end_pos == npos) {
        PARSE_ERROR();
    }
    return sub.substr(0, end_pos + 1);
}

HttpText& HttpText::Rewind()
{
    mCursor = 0;
    return *this;
}

HttpText HttpText::Line()
{
    auto pos = mSource.find(cCRLF, mCursor);
    if (pos == npos) {
        PARSE_ERROR();
    }
    auto start = mCursor;
    mCursor = pos + 2;
    return HttpText(mSource.substr(start, mCursor - start));
}

bool HttpText::IsNewLine() const
{
    if ((mCursor+2) > mSource.size()) {
        return false;
    }
    return (mSource.at(mCursor) == '\r') && (mSource.at(mCursor+1) == '\n');
}

bool HttpText::Eof() const
{
    return (mCursor >= mSource.size());
}

} // rsp::network
