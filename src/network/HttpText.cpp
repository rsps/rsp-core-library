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

namespace rsp::network {

HttpText::HttpText(std::string_view aSource)
    : mSource(aSource)
{
}

HttpText& HttpText::CR()
{
    if (mSource.at(mCursor) != '\r') {
        THROW_WITH_BACKTRACE(EHttpParseError);
    }
    mCursor++;
    return *this;
}

HttpText& HttpText::CRLF()
{
    if (mSource.find("\r\n", mCursor) != mCursor) {
        THROW_WITH_BACKTRACE(EHttpParseError);
    }
    mCursor += 2;
    return *this;
}

HttpText& HttpText::LF()
{
    if (mSource.at(mCursor) != '\n') {
        THROW_WITH_BACKTRACE(EHttpParseError);
    }
    mCursor++;
    return *this;
}

HttpText& HttpText::OWS()
{
    mCursor = mSource.find_first_not_of(" \t", mCursor);
    return *this;
}

HttpText& HttpText::RWS(size_t aCount)
{
    if (mSource.substr(mCursor, aCount).find_first_not_of(" \t") != npos) {
        THROW_WITH_BACKTRACE(EHttpParseError);
    }
    mCursor += aCount;
    return *this;
}

std::string_view HttpText::Alpha(size_t aSize)
{
    auto sub = mSource.substr(mCursor, aSize);
    for (auto c : sub) {
        if (!(c >= 'A' && c <= 'Z') && !(c >= 'a' && c <= 'z')) {
            THROW_WITH_BACKTRACE(EHttpParseError);
        }
    }
    mCursor += sub.size();
//    return sub;
    return { sub.data(), sub.size() };
}

int HttpText::Digit(size_t aCount)
{
    auto sub = mSource.substr(mCursor, aCount);
    for (auto c : sub) {
        if (!(c >= '0' && c <= '9')) {
            THROW_WITH_BACKTRACE(EHttpParseError);
        }
    }
    mCursor += sub.size();
    auto result = string_to_integral<int>(sub);
    if (!result) {
        THROW_WITH_BACKTRACE(EHttpParseError);
    }
    return *result;
}

int HttpText::HexDigit(size_t aCount)
{
    auto sub = mSource.substr(mCursor, aCount);
    for (auto c : sub) {
        if (!(c >= '0' && c <= '9') && !(c >= 'A' && c <= 'F') && !(c >= 'a' && c <= 'f')) {
            THROW_WITH_BACKTRACE(EHttpParseError);
        }
    }
    mCursor += sub.size();
    auto result = string_to_integral<int>(sub, 16);
    if (!result) {
        THROW_WITH_BACKTRACE(EHttpParseError);
    }
    return *result;
}

std::span<const std::byte> HttpText::Octets(size_t aSize)
{
    auto sub = mSource.substr(mCursor, aSize);
    mCursor += sub.size();
    return { reinterpret_cast<const std::byte*>(sub.data()), sub.size() };
}

std::string_view HttpText::Key()
{
    // TODO: Finish from here....
    // \see https://datatracker.ietf.org/doc/html/rfc7230#section-4.1.2
    // \see https://datatracker.ietf.org/doc/html/rfc7540#section-8.1
    return std::string_view();
}

std::string_view HttpText::Value()
{
    return std::string_view();
}

HttpText& HttpText::Rewind()
{
    mCursor = 0;
    return *this;
}
} // rsp::network