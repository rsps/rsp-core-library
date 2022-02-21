/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_JSON_JSON_H_
#define INCLUDE_UTILS_JSON_JSON_H_

#include <sstream>
#include <vector>
#include <map>
#include <memory>
#include "JsonExceptions.h"
#include "JsonValue.h"
#include "JsonArray.h"
#include "JsonObject.h"

namespace rsp::utils::json {

class Json
{
public:
    Json() : mpValue{nullptr} {}
    Json(const Json &arOther);
    Json(Json &&arOther);
    virtual ~Json();

    Json& operator=(const Json &arOther);
    Json& operator=(Json &&arOther);

    JsonObject& MakeObject();
    JsonArray& MakeArray();

    void Decode(const std::string &arJson);
    std::string Encode(bool aPrettyPrint = false) const;

    JsonValue& operator*();
    JsonValue* operator->();

    Json& Clear();
    bool Empty() const { return (!mpValue); }
protected:
    JsonValue *mpValue;
};

} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSON_H_ */
