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

namespace rsp::utils::json {

class JsonObject;
class JsonArray;

enum class JsonTypes : unsigned int { Null, Bool, Number, String, Object, Array };

class JsonValue : public Variant
{
public:
    JsonValue() : Variant() {}
    // Use template to call inherited constructors
    template<class T>
    JsonValue(T aValue) : Variant(aValue) {}

    JsonValue(const JsonValue&);
    JsonValue(const JsonValue&&);
    virtual ~JsonValue();

    JsonValue& operator=(const JsonValue&);
    JsonValue& operator=(const JsonValue&&);

    std::string Encode(bool aPrettyPrint = false, bool aForceToUCS2 = false);

    JsonTypes GetJsonType() const;

// Disable wrong effc++ warning on "return *this;" in template functions. Fixed in GCC 11.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
    // Use template to call inherited assignment operators
    template<class T>
    JsonValue& operator=(T aValue) { Variant::operator=(aValue); return *this; }
#pragma GCC diagnostic pop

    virtual bool IsArray() const { return false; }
    virtual bool IsObject() const { return false; }
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

    friend JsonArray;
    friend JsonObject;
    std::string jsonTypeToText() const;

    virtual void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2);

    static bool mEncodeToUCS2;
};


} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONVALUE_H_ */
