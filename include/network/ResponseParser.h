/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_RESPONSE_PARSER_H
#define RSP_CORE_LIB_RESPONSE_PARSER_H

#include "HttpResponse.h"
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace rsp::network {

class ResponseParser
{
public:
    explicit ResponseParser(HttpResponse& arResponse) : mrResponse(arResponse) {}

    bool ParseNewData(std::span<const std::byte> aNewData);

protected:
    enum class States {
        Headers,
        Body,
        ChunkedBody,
        ChunkedTrail
    };
    HttpResponse& mrResponse;
    size_t mContentReceived = 0;
    States mState = States::Headers;

    void decodeHeaders(std::string_view aHeaderData);
    void addHeader(std::string_view aKey, std::string_view aValue);
};

} // rsp::network

#endif //RSP_CORE_LIB_RESPONSE_PARSER_H
