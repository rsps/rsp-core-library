/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <rsp/network/StringBody.h>
#include <rsp/network/HttpText.h>
#include <rsp/network/parser-helpers.h>
#include <rsp/network/ResponseParser.h>

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
                auto body = work.substr(position + cHeaderEnd.size());
                if (mrResponse.GetHeaders().contains("transfer-encoding") && mrResponse.GetHeader("transfer-encoding").ends_with("chunked")) {
                    mState = States::ChunkedBody;
                    return decodeChunkedBody(body);
                }
                else {
                    mState = States::Body;
                    if (!body.empty()) {
                        mContentReceived += mrResponse.mpBody->Write({reinterpret_cast<const std::byte*>(body.data()), body.size()});
                    }
                    return mrResponse.mContentLength == mContentReceived;
                }
            }
            break;

        case States::Body:
            mContentReceived += mrResponse.mpBody->Write(aNewData);
            return mrResponse.mContentLength == mContentReceived;

        case States::ChunkedBody:
            if (decodeChunkedBody({reinterpret_cast<const char*>(aNewData.data()), aNewData.size()})) {
                mState = States::ChunkedTrail;
                return decodeChunkedTrailer("");
            }
            break;

        case States::ChunkedTrail:
            return decodeChunkedTrailer({reinterpret_cast<const char*>(aNewData.data()), aNewData.size()});
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

    if (mrResponse.GetStatusCode() == StatusCodes::NoContent) {
        mrResponse.mContentLength = 0;
    }
    else if (auto it = mrResponse.mHeaders.find("content-length"); it != mrResponse.mHeaders.end()) {
        mrResponse.mContentLength = string_to_integral<size_t>(it->second).value_or(0);
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

bool ResponseParser::decodeChunkedBody(std::string_view aData)
{
    if (aData.empty()) {
        return false;
    }
    mContentReceived += aData.size();
    mChunkData += aData;
    if (mChunkData.size() < 2) { // Wait for at least a cr+lf
        return false;
    }

    try {
        while (true) {
            auto ht = HttpText(mChunkData);
            auto line = ht.Line(); // Throws if line is not complete
            auto line_size = ht.GetCursor();

            size_t mChunkLength = line.HexDigit();
            decodeChunkExtension(line);

            if (mChunkLength == 0) { // EOF
                mChunkData = mChunkData.substr(line_size);
                return true;
            }

            if (mChunkLength <= ht.GetRemaining().size()) {
                auto chunk_data = ht.Octets(mChunkLength);
                ht.CRLF(); // Body must end with cr+lf to be valid.
                auto written = mrResponse.mpBody->Write(chunk_data);
                mChunkData = mChunkData.substr(written + line_size + 2); // Remove parsed part from mChunkData
                continue; // loop around and try again with remaining part
            }

            break;
        }
    }
    catch (const std::out_of_range &e) {
        // Ignore std::string_view out of range errors, data could be incomplete
    }
    catch (const EHttpParseError &e) {
        // Ignore parser errors, data could be incomplete
    }

    return false;
}

void ResponseParser::decodeChunkExtension(HttpText& arLine)
{
    arLine.OWS();
    auto opt = mrResponse.GetRequest().GetOptions();
    while (auto pair = arLine.OChunkExt()) {
        if (opt.ResponseChunkExtHandler) {
            opt.ResponseChunkExtHandler(pair->first, pair->second);
        }
    }
}

bool ResponseParser::decodeChunkedTrailer(std::string_view aData)
{
    mContentReceived += aData.size();
    mChunkData += aData;
    if (mChunkData.size() < 2) { // Wait for at least a cr+lf
        return false;
    }

    auto ht = HttpText(mChunkData);

    try {
        while (auto line = ht.Line()) {
            if (line.IsNewLine()) {
                return true; // Empty line is reached
            }
            auto key = line.FieldName();
            addHeader(key, line.FieldValue());
        }
    }
    catch(const EHttpParseError &e) {
        // Ignore parser errors, data could be incomplete
    }
    mChunkData = mChunkData.substr(ht.GetCursor());

    return false;
}

} // rsp::network
