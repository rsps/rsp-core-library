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
#include <utils/CsvEncoder.h>

namespace rsp::utils {

CsvEncoder::CsvEncoder(bool aIncludeHeaders, char aSeparator, bool aPrettyPrint)
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
            makeHeaders(arData);
        }

        std::string sep;
        for (auto &name : mHeaders) {
            result << sep << format(name);
            sep = mSeparator;
        }
        result << "\n";
    }

    for (size_t i = 0; i < arData.GetCount() ;++i) {
        // TODO: Stream data recursively...
        streamRow(result, arData[i]);
        result << "\n";
    }

    return result.str();
}

CsvEncoder& CsvEncoder::SetHeaders(std::vector<std::string> aHeaders)
{
    mHeaders = std::move(aHeaders);
    return *this;
}

void CsvEncoder::makeHeaders(const DynamicData &arData)
{
    if (arData.IsObject()) {
        std::string_view sep;
        const DynamicData &h = arData["Headers"];
        for (size_t i = 0; i < h.GetCount() ; ++i) {
            mHeaders.push_back(h[i].AsString());
        }
    }
    else if (arData.IsArray()) {
        for (auto &name : arData[0].GetMemberNames()) {
            mHeaders.push_back(name);
        }
    }
}

std::string CsvEncoder::format(const DynamicData &arValue) const
{
    if (arValue.IsArray()) {
        return "array";
    }
    else if(arValue.IsObject()) {
        return "object";
    }
    else if (arValue.IsNull()) {
        return "";
    }

    std::string value = arValue.AsString();

    if (mPrettyPrint && value == "NotAvailable") {
        return "N/A";
    }
    std::string result;
    result.reserve(result.size() + 5);
    bool first = true;
    bool need_quote = false;
    for (char chr : value) {
        if (mPrettyPrint && std::isupper(chr) && !first && result.back() != ' ') {
            result += ' ';
        }
        if ((chr == '\n') || (chr == mSeparator)) {
            need_quote = true;
        }
        else if (chr == '"') {
            need_quote = true;
            result += '"';
        }
        result += chr;
        first = false;
    }
    if (need_quote) {
        result = "\"" + result + "\"";
    }
    return result;
}

void CsvEncoder::streamRow(std::ostream &arResult, const DynamicData &arRow)
{
    std::string separator;
    if (arRow.IsObject()) {
        for (auto &name : arRow.GetMemberNames()) {
            arResult << separator << format(arRow[name]);
            separator = mSeparator;
        }

    }
    else if (arRow.IsArray()) {
        for (size_t i = 0; i < arRow.GetCount(); ++i) {
            arResult << separator << format(arRow[i]);
            separator = mSeparator;
        }
    }
}

} // rsp::utils
