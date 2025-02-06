/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
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
                decodeHeaders({ mRemaining.data(), position + 2 }); // Include newline before empty line
                mRemaining = mRemaining.substr(position);
                if (mrResponse.GetHeaders().contains("transfer-encoding") && mrResponse.GetHeader("transfer-encoding") == "chunked") {
                    mState = States::ChunkedBody;
                }
                else {
                    mState = States::Body;
                    mrResponse.mBody = std::move(mRemaining);
                    mRemaining.clear();
                    result = std::stoul(mrResponse.GetHeader("content-length")) == mrResponse.mBody.size();
                }
            }
            break;

        case States::Body:
            mrResponse.mBody = std::move(mRemaining);
            mRemaining.clear();
            result = std::stoul(mrResponse.GetHeader("content-length")) == mrResponse.mBody.size();
            break;

        case States::ChunkedBody:
        case States::ChunkedTrail:
            break;
    }

    return result;
}

void ResponseParser::decodeHeaders(std::string_view aHeaderData)
{
    auto position = aHeaderData.find(cNewLine);
    if (position != std::string_view::npos) {
        mrResponse.mStatusLine = StatusLine({aHeaderData.data(), position });

        if (auto number = string_to_integral<int>(mrResponse.mStatusLine.GetStatusCode())) {
            mrResponse.mStatusCode = StatusCodes(*number);
        }
    }

    while(true) {
        auto end_position = aHeaderData.find(cNewLine, position);
        if (end_position == std::string_view::npos) {
            break; // Finished
        }
        addHeader(std::string(aHeaderData.substr(position, end_position - position)));
        position = end_position + 2;
    }

}

void ResponseParser::addHeader(std::string_view aHeaderLine)
{
    std::string header(aHeaderLine);
    size_t separator = header.find_first_of(':');
    if (std::string::npos == separator) {
        StrUtils::ToLower(StrUtils::Trim(header));
        if (header.empty()) {
            return; // blank line;
        }
        mrResponse.mHeaders[header] = "present";
    }
    else {
        std::string key = header.substr(0, separator);
        StrUtils::ToLower(StrUtils::Trim(key));
        std::string value = header.substr(separator + 1);
        StrUtils::Trim(value);
        if (key == "content-length" && mrResponse.mHeaders.contains(key) && mrResponse.mHeaders.at(key) != value) {
            //
        }
        mrResponse.mHeaders[key] = value;
    }
}

} // rsp::network