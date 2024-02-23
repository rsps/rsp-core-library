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
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace rsp::utils {

class CsvEncoder : public rsp::utils::DynamicData::Encoder
{
public:
    explicit CsvEncoder(bool aIncludeHeaders = true, char aSeparator = ',', bool aPrettyPrint = true);

    CsvEncoder& SetHeaders(std::vector<std::string> aHeaders);
    std::string Encode(const DynamicData &arData) override;

protected:
    bool mIncludeHeaders;
    char mSeparator;
    bool mPrettyPrint;
    std::vector<std::string> mHeaders{};

    void makeHeaders(const DynamicData &arData);
    [[nodiscard]] std::string format(const DynamicData &arValue) const;
    void streamRow(std::ostream &arResult, const DynamicData &arRow);
};

} // rsp::utils

#endif //RSP_CORE_LIB_CSVENCODER_H
