/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_CSVENCODER_H
#define RSP_CORE_LIB_CSVENCODER_H

#include "DynamicData.h"
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace rsp::utils {

class CsvEncoder : public rsp::utils::DynamicData::Encoder
{
public:
    explicit CsvEncoder(bool aIncludeHeaders = true, std::string aSeparator = ",", bool aPrettyPrint = true);

    CsvEncoder& SetHeaders(std::vector<std::string> aHeaders);
    std::string Encode(const DynamicData &arData) override;

protected:
    bool mIncludeHeaders;
    std::string mSeparator;
    bool mPrettyPrint;
    std::vector<std::string> mHeaders{};
    std::map<std::string, std::uint32_t > mObjectHeadersCount;

    void makeHeaders(const DynamicData &arData, const std::string &arKey, bool &arFoundNull);
    [[nodiscard]] std::string format(std::string_view aString) const;
};

} // rsp::utils

#endif //RSP_CORE_LIB_CSVENCODER_H
