/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <network/StringBody.h>
#include <network/HttpText.h>
#include <network/parser-helpers.h>
#include <network/ResponseParser.h>

using namespace std::string_view_literals;
using namespace rsp::utils;

namespace rsp::network {

bool ResponseParser::ParseNewData(std::span<const std::byte> aNewData)
{
    auto work = std::string_view(reinterpret_cast<const char*>(aNewData.data()), aNewData.size());

    switch (mState) {
        case States::Headers:
            if (auto position = work.find(cHeaderEnd); position == std::string::npos) {
                mrResponse.mHeaderData += work;
            }
            else {
                mrResponse.mHeaderData += work.substr(0, position + cHeaderEnd.size());
                decodeHeaders(mrResponse.mHeaderData); // Include newline before empty line
                mrResponse.MakeBody();
                mContentReceived = 0;
                if (mrResponse.GetRequest().GetOptions().RequestType == HttpRequestType::HEAD) {
                    return true;
                }
                (void)mrResponse.GetContentLength(); // Attempt to parse content-length from headers.
                if (mrResponse.GetHeaders().contains("transfer-encoding") && mrResponse.GetHeader("transfer-encoding").ends_with("chunked")) {
                    mState = States::ChunkedBody;
                }
                else {
                    mState = States::Body;
                    auto body = work.substr(position + cHeaderEnd.size());
                    mContentReceived+= mrResponse.mpBody->Write({ reinterpret_cast<const std::byte*>(body.data()), body.size() });
                    return mrResponse.mContentLength == mContentReceived;
                }
            }
            break;

        case States::Body:
            mContentReceived += mrResponse.mpBody->Write(aNewData);
            return mrResponse.mContentLength == mContentReceived;

        case States::ChunkedBody:
            // TODO: Implement chunked support. Format: 0x<length>\r\n<body part>\r\n
        case States::ChunkedTrail:
            break;
    }

    return false;
}

void ResponseParser::decodeHeaders(std::string_view aHeaderData)
{
    auto ht = HttpText(aHeaderData);
    mrResponse.mStatusLine = StatusLine(ht.Line());

    while (auto line = ht.Line()) {
        if (line.IsNewLine()) {
            break; // Empty line is reached
        }
        auto key = line.FieldName();
        addHeader(key, line.FieldValue());
    }
}

void ResponseParser::addHeader(std::string_view aKey, std::string_view aValue)
{
    if (equal_ascii_case_insensitive(aKey, "content-length")) {
        if (auto it = mrResponse.mHeaders.find(aKey); it != mrResponse.mHeaders.end()) {
            if (it->second != aValue) {
                THROW_WITH_BACKTRACE1(EHttpParseError, "Multiple Content-Length given with different values.");
            }
        }
    }

    mrResponse.mHeaders.try_emplace(aKey, aValue);
}

} // rsp::network