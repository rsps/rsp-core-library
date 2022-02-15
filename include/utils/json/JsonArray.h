/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_UTILS_JSON_JSONARRAY_H_
#define INCLUDE_UTILS_JSON_JSONARRAY_H_

#include <vector>
#include "JsonValue.h"

namespace rsp::utils::json {


class JsonArray : public JsonValue
{
public:
    JsonArray();
    ~JsonArray() override;

    std::size_t GetCount() const;
    JsonValue& operator[](unsigned int aIndex);

    JsonArray& Add(JsonValue* apValue);
    JsonArray& Remove(int aIndex);
    void Clear() override;

protected:
    std::vector<JsonValue*> mData{};

    void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel) override;
};


} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONARRAY_H_ */
