/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <network/HttpText.h>
#include <network/parser-helpers.h>
#include <cctype>

using namespace std::string_view_literals;

namespace rsp::network {

constexpr auto cDelimiters = R"("(),/:;<=>?@[\]{})"sv;
constexpr auto cCRLF = "\r\n"sv;
constexpr auto cSpaceTab = " \t"sv;

#define PARSE_ERROR() THROW_WITH_BACKTRACE2(EHttpParseError, mSource, mCursor);


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

HttpText& HttpText::OWS()
{
    mCursor = mSource.find_first_not_of(cSpaceTab, mCursor);
    return *this;
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
//    return { sub.data(), sub.size() };
}

int HttpText::Digit(size_t aCount)
{
    auto sub = mSource.substr(mCursor, aCount);
    for (auto c : sub) {
        if (!std::isdigit(c)) {
            PARSE_ERROR();
        }
    }
    mCursor += sub.size();
    auto result = string_to_integral<int>(sub);
    if (!result) {
        PARSE_ERROR();
    }
    return *result;
}

int HttpText::HexDigit(size_t aCount)
{
    auto sub = mSource.substr(mCursor, aCount);
    for (auto c : sub) {
        if (!std::isxdigit(c)) {
            PARSE_ERROR();
        }
    }
    mCursor += sub.size();
    auto result = string_to_integral<int>(sub, 16);
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

    auto sub = mSource.substr(mCursor, colon_pos);
    for (auto c : sub) {
        if (!std::isgraph(c)) {
            PARSE_ERROR();
        }
        if (cDelimiters.contains(c)) {
            PARSE_ERROR();
        }
    }
    mCursor = colon_pos + 1;
    return sub;
}

std::string_view HttpText::FieldValue()
{
    OWS();
    if (mSource.at(mCursor) == '"') {
        return quotedString();
    }
    auto sub = mSource.substr(mCursor);
    auto end_pos = sub.find_last_not_of(cSpaceTab);
    mCursor += sub.size();
    return sub.substr(0, end_pos);
}

std::string_view HttpText::quotedString()
{
    if (mSource.at(mCursor) != '"') {
        PARSE_ERROR();
    }
    auto end_pos = mSource.find_last_not_of(cSpaceTab);
    if (end_pos == npos || mSource.at(end_pos) != '"') {
        PARSE_ERROR();
    }
    auto start = mCursor;
    mCursor = end_pos + 1;
    return mSource.substr(start, end_pos);
}

HttpText& HttpText::Rewind()
{
    mCursor = 0;
    return *this;
}

HttpText HttpText::Line()
{
    auto pos = mSource.find(cCRLF, mCursor);
    auto start = mCursor;
    mCursor += pos + 2;
    return HttpText(mSource.substr(start, pos));
}

bool HttpText::IsNewLine() const
{
    if ((mCursor+2) >= mSource.size()) {
        return false;
    }
    return (mSource.at(mCursor) != '\r') && (mSource.at(mCursor+1) != '\n');
}

bool HttpText::Eof() const
{
    return (mCursor >= mSource.size());
}

} // rsp::network