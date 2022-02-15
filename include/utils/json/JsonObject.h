/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_UTILS_JSON_JSONOBJECT_H_
#define INCLUDE_UTILS_JSON_JSONOBJECT_H_

#include <map>
#include "JsonValue.h"

namespace rsp::utils::json {

class JsonObject : public JsonValue
{
public:
    JsonObject();
    ~JsonObject() override;

    std::size_t GetCount() const;
    bool MemberExists(const std::string &aName) const;

    JsonValue& operator[](const char *apName);
    JsonValue& operator[](const std::string &aName);

    JsonObject& Add(const std::string &aName, JsonValue* apValue);
    JsonObject& Remove(const std::string &aName);
    void Clear() override;

protected:
    std::map<const std::string, JsonValue*> mData{};

    void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel) override;
};


} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONOBJECT_H_ */
