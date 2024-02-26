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
#include <utils/InRange.h>

namespace rsp::utils {

CsvEncoder::CsvEncoder(bool aIncludeHeaders, char aSeparator, bool aPrettyPrint)
    : mIncludeHeaders(aIncludeHeaders),
      mSeparator(aSeparator),
      mPrettyPrint(aPrettyPrint)
{
}

std::string CsvEncoder::Encode(const DynamicData &arData)
{
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

    if (arData.IsArray()) {
        for (size_t i = 0; i < arData.GetCount() ;++i) {
            streamRow(result, arData[i]);
            result << "\n";
        }
    }
    else if (arData.IsObject() && arData.MemberExists("Data") && arData["Data"].IsArray()) {
        auto &data = arData["Data"];
        for (size_t i = 0; i < data.GetCount() ;++i) {
            streamRow(result, data[i]);
            result << "\n";
        }
    }
    else {
        THROW_WITH_BACKTRACE1(ENotAnArray, "CsvEncoder data is not an array");
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
    if (arData.IsObject() && arData.MemberExists("Headers")) {
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
    std::string value;
    if (mFormatter) {
        value = mFormatter(arValue);
    }
    else if (arValue.IsArray()) {
        return "array";
    }
    else if(arValue.IsObject()) {
        return "object";
    }
    else if (arValue.IsNull()) {
        return "";
    }
    else {
        value = arValue.AsString();
    }

    if (mPrettyPrint && value == "NotAvailable") {
        return "N/A";
    }
    std::string result;
    result.reserve(result.size() + 5);
    bool first = true;
    bool need_quote = false;
    for (char chr : value) {
        if (mPrettyPrint && std::isupper(chr) && !first && IsInRange(result.back(), 'a', 'z')) {
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
    else { // Only one element on row
        arResult << format(arRow);
    }
}

CsvEncoder &CsvEncoder::SetValueFormatter(CsvEncoder::ValueFormatter_t aFormatter)
{
    mFormatter = std::move(aFormatter);
    return *this;
}

} // rsp::utils
