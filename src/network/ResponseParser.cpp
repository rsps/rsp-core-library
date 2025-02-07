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
#include <utils/StrUtils.h>

using namespace std::string_view_literals;
using namespace rsp::utils;

namespace rsp::network {

bool ResponseParser::ParseNewData(std::span<std::byte> aNewData)
{
    bool result = false;

    mRemaining += std::string(reinterpret_cast<char*>(aNewData.data()), aNewData.size());

    switch (mState) {
        case States::Headers:
            if (auto position = mRemaining.find(cHeaderEnd); position != std::string::npos) {
                mrResponse.mHeaderData = mRemaining.substr(0, position + 2);
                decodeHeaders(mrResponse.mHeaderData); // Include newline before empty line
                mrResponse.MakeBody();
                (void)mrResponse.GetContentLength(); // Attempt to parse content-length from headers.
                mRemaining = mRemaining.substr(position);
                if (mrResponse.GetHeaders().contains("transfer-encoding") && mrResponse.GetHeader("transfer-encoding").ends_with("chunked")) {
                    mState = States::ChunkedBody;
                }
                else {
                    mState = States::Body;
                    mrResponse.mpBody->Write({ reinterpret_cast<const std::byte*>(mRemaining.data()), mRemaining.size() });
                    mRemaining.clear();
                    result = mrResponse.mContentLength == mrResponse.mpBody->GetStreamSize();
                }
            }
            break;

        case States::Body:
            mrResponse.mpBody->Write({ reinterpret_cast<const std::byte*>(mRemaining.data()), mRemaining.size() });
            mRemaining.clear();
            result = mrResponse.mContentLength == mrResponse.mpBody->GetStreamSize();
            break;

        case States::ChunkedBody:
        case States::ChunkedTrail:
            break;
    }

    return result;
}

void ResponseParser::decodeHeaders(std::string_view aHeaderData)
{
    auto ht = HttpText(aHeaderData);
    mrResponse.mStatusLine = StatusLine(std::string(ht.Line().Source()));

    while (auto line = ht.Line()) {
        addHeader(line.FieldName(), line.FieldValue());
    }
}

void ResponseParser::addHeader(std::string_view aKey, std::string_view aValue)
{
    std::string key(aKey);
    StrUtils::ToLower(key);
    std::string value(aValue);
    mrResponse.mHeaders.try_emplace(key, value);

    if (key == "content-length" && mrResponse.mHeaders.at(key) != value) {
        THROW_WITH_BACKTRACE1(EHttpParseError, "Multiple Content-Length given with different values.");
    }
}

} // rsp::network