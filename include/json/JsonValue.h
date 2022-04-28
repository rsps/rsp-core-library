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
class Json;

enum class JsonTypes : unsigned int { Null, Bool, Number, String, Object, Array };

/**
 * \class JsonValue
 * \brief Class to hold all JSON value types
 */
class JsonValue : public Variant
{
public:
    /**
     * \fn JsonValue()
     * \brief Constructs a null value
     */
    JsonValue() : Variant() {}
    JsonValue(const JsonValue&);
    JsonValue(const JsonValue&&);
    /**
     * \brief Construct a JsonValue holding the given value
     * \tparam T Type of value to contain
     * \param aValue Value to contain
     */
    // Use template to call inherited constructors
    template<class T>
    JsonValue(T aValue) : Variant(aValue) {}

    virtual ~JsonValue();

    JsonValue& operator=(const JsonValue&);
    JsonValue& operator=(const JsonValue&&);

    /**
     * \brief Encode this object and all its children to a JSON formatted string
     * \param aPrettyPrint Set to make the string human readable
     * \param aForceToUCS2 Set to use UCS2 codepoints for all characters above ASCII.
     * \return JSON formatted string
     */
    std::string Encode(bool aPrettyPrint = false, bool aForceToUCS2 = false);

    /**
     * \brief Get the type of the value content
     * \return JsonTypes
     */
    JsonTypes GetJsonType() const;

    /**
     * \brief Get the type as a string. Useful for error logging.
     * \return string
     */
    std::string GetJsonTypeAsString() const;

// Disable wrong effc++ warning on "return *this;" in template functions. Fixed in GCC 11.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
    // Use template to call inherited assignment operators
    template<class T>
    JsonValue& operator=(T aValue) { Variant::operator=(aValue); return *this; }
#pragma GCC diagnostic pop

    /**
     * \brief Check if value content is an array
     * \return True if content is an array
     */
    virtual bool IsArray() const { return false; }
    /**
     * \brief Check if value content is an object
     * \return True if content is an object
     */
    virtual bool IsObject() const { return false; }

    /**
     * \brief Access the content as a JsonArray reference.
     * \return Reference to this cast to JsonArray
     */
    JsonArray& AsArray() const;
    /**
     * \brief Access the content as a JsonObject reference.
     * \return Reference to this cast to JsonObject
     */
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
    friend Json;
    virtual void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2);
    virtual JsonValue* clone() const;

    static bool mEncodeToUCS2;
};


} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONVALUE_H_ */
