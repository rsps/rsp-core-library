/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_UTILS_JSON_JSONVALUE_H_
#define INCLUDE_UTILS_JSON_JSONVALUE_H_

#include <utils/Variant.h>
#include "JsonString.h"

namespace rsp::utils::json {

class JsonObject;
class JsonArray;

enum class JsonTypes : unsigned int { Null, Bool, Number, String, Object, Array };

class JsonValue : public Variant
{
public:
//    static JsonValue* Decode(const char* apJson) { JsonString js(apJson); return Decode(js); }
//    static JsonValue* Decode(const JsonString &arJson) { JsonString js(arJson); return Decode(js); }
//    static JsonValue* Decode(JsonString &arJson);
    JsonString Encode(bool aPrettyPrint = false);

    JsonTypes GetJsonType() const;

    virtual ~JsonValue();
    JsonValue(const JsonValue&) = delete;
    JsonValue& operator=(const JsonValue&) = delete;

    JsonValue() : Variant() {}

    // Use template to call inherited constructors
    template<class T>
    JsonValue(T aValue) : Variant(aValue) {}

// Disable wrong effc++ warning on "return *this;" in template functions. Fixed in GCC 11.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
    // Use template to call inherited assignment operators
    template<class T>
    JsonValue& operator=(T aValue) { Variant::operator=(aValue); return *this; }
#pragma GCC diagnostic pop

    JsonArray& AsArray() const;
    JsonObject& AsObject() const;

protected:
    class PrintFormat {
    public:
        PrintFormat() : indent(0) {}
        PrintFormat(unsigned int aIndent, const char* aNl, const char* aSp) : indent(aIndent), nl(aNl), sp(aSp) {}
        unsigned int indent;
        std::string nl{};
        std::string sp{};
    };

    std::string jsonTypeToText() const;

    virtual void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel);

private:
    friend class JsonArray;
    friend class JsonObject;

//    static void jWalk(const char aToken1, const char aToken2, UTF8String &arJson, UTF8String &arResult);
//    static std::string::iterator whitespace(std::string::iterator aIt);
//    static UTF8String parseString(UTF8String &arJson);
//    static void parseNumber(UTF8String &arJson, JsonValue* apResult);
};


} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONVALUE_H_ */
