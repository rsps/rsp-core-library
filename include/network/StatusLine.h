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

#include <string>
#include <string_view>

namespace rsp::network {

class StatusLine
{
public:
    StatusLine() = default;

    explicit StatusLine(std::string aStatusLine)
        : mStatusLine(std::move(aStatusLine))
    {
        auto sp1 = mStatusLine.find(' ');
        if (sp1 == std::string::npos) {
            return;
        }
        mHttpVersion = {mStatusLine.data(), sp1 };

        sp1 += 1;
        auto sp2 = mStatusLine.find(' ', sp1);
        if (sp2 == std::string::npos) {
            return;
        }
        mStatusCode = {mStatusLine.data() + sp1, sp2 };
        sp2 += 1;

        mReasonPhrase = {mStatusLine.data() + sp2, sp2 - mStatusLine.size() };
    }

    [[nodiscard]] std::string_view GetHttpVersion() const { return mHttpVersion; }
    [[nodiscard]] std::string_view GetStatusCode() const { return mStatusCode; }
    [[nodiscard]] std::string_view GetReasonPhrase() const { return mReasonPhrase; }

protected:
    std::string mStatusLine{};
    std::string_view mHttpVersion{};
    std::string_view mStatusCode{};
    std::string_view mReasonPhrase{};
};

} // namespace rsp::network

#endif //RSP_CORE_LIB_STATUS_LINE_H
