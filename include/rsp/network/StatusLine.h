/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_STATUS_LINE_H
#define RSP_CORE_LIB_STATUS_LINE_H

#include "HttpText.h"

namespace rsp::network {

class StatusLine
{
public:
    StatusLine() = default;

    explicit StatusLine(HttpText aStatusLine)
    {
        mHttpVersion = aStatusLine.HttpVersion();
        mStatusCode = static_cast<int>(aStatusLine.Digit(3));
        aStatusLine.SP();
        mReasonPhrase = aStatusLine.AsciiText();
    }

    [[nodiscard]] std::string_view GetHttpVersion() const { return mHttpVersion; }
    [[nodiscard]] int GetStatusCode() const { return mStatusCode; }
    [[nodiscard]] std::string_view GetReasonPhrase() const { return mReasonPhrase; }

protected:
    std::string_view mHttpVersion{};
    int mStatusCode{};
    std::string_view mReasonPhrase{};
};

} // namespace rsp::network

#endif //RSP_CORE_LIB_STATUS_LINE_H
