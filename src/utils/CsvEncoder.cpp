/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <exceptions/CoreException.h>
#include "utils/CsvEncoder.h"

namespace rsp::utils {

CsvEncoder::CsvEncoder(bool aIncludeHeaders, std::string aSeparator, bool aPrettyPrint)
    : mIncludeHeaders(aIncludeHeaders),
      mSeparator(aSeparator),
      mPrettyPrint(aPrettyPrint)
{
}

std::string CsvEncoder::Encode(const DynamicData &arData)
{
    if (!arData.IsArray()) {
        THROW_WITH_BACKTRACE1(exceptions::ENotAnArray, "CsvEncoder data is not an array");
    }
    std::stringstream result;
    if (mIncludeHeaders) {
        if (mHeaders.empty()) {
            bool found_null = false;
            for (size_t i = 0; i < arData.GetCount(); ++i) {
                makeHeaders(arData[i], "", found_null);
                if (!found_null) {
                    break; // No unknown members. We are done.
                }
            }
        }

        std::string_view sep = "";
        for (auto &name : mHeaders) {
            result << sep << "\"" << format(name) << "\"";
            sep = mSeparator;
        }
        result << "\n";
    }

    for (size_t i = 0; i < arData.GetCount() ;++i) {
        // TODO: Stream data recursively...
        streamValue(result, arData[i]);
    }

    return std::string();
}

CsvEncoder& CsvEncoder::SetHeaders(std::vector<std::string> aHeaders)
{
    mHeaders = std::move(aHeaders);
    return *this;
}

void CsvEncoder::makeHeaders(const DynamicData &arData, const std::string &arKey, bool &arFoundNull)
{
    if (!arKey.empty()) {
        mHeaders.push_back(arKey);
    }

    if (arData.IsNull()) {
        arFoundNull = true;
    }
    else if (arData.IsObject()) {
        for (auto &name : arData.GetMemberNames()) {
            makeHeaders(arKey[name], name, arFoundNull);
        }
    }
    else if (arData.IsArray()) {
        for (size_t i = 0; i < arData.GetCount(); ++i) {
            makeHeaders(arKey[i], "", arFoundNull);
        }
    }
}

std::string CsvEncoder::format(std::string_view aString) const
{
    if (!mPrettyPrint) {
        return "\"" + std::string(aString) + "\"";
    }
    if (aString == "NotAvailable") {
        return "N/A";
    }
    std::string result;
    result.reserve(aString.size() + 5);
    bool first = true;
    bool need_quote = false;
    for (char chr : aString) {
        if (std::isupper(chr) && !first) {
            result += " ";
        }
        if (chr == '"' || chr == '\n') {
            need_quote = true;
        }
        result += chr;
        first = false;
    }
    if (need_quote) {
        result = "\"" + result + "\"";
    }
    return result;
}

} // rsp::utils
