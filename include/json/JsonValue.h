/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_JSON_JSONVALUE_H_
#define INCLUDE_JSON_JSONVALUE_H_

#include <string_view>
#include <utils/Variant.h>
#include <utils/InsertOrderedMap.h>

namespace rsp::json {

class JsonDecoder;

enum class JsonTypes : unsigned int { Null, Bool, Number, String, Object, Array };

std::ostream& operator<<(std::ostream& os, JsonTypes aType);


/**
 * \class JsonValue
 * \brief Class to hold all JSON value types
 */
class JsonValue : public rsp::utils::Variant
{
public:
    /**
     * \fn JsonValue()
     * \brief Constructs a null value
     */
    JsonValue() : Variant() {}
    JsonValue(JsonTypes aType);

    JsonValue(const JsonValue&);
    JsonValue(JsonValue&&);
    /**
     * \brief Construct a JsonValue holding the given value
     * \tparam T Type of value to contain
     * \param aValue Value to contain
     *
     * Use template to call inherited constructors
     */
    template<class T>
    JsonValue(T aValue) : Variant(aValue) {}

    virtual ~JsonValue();

    JsonValue& operator=(const JsonValue&);
    JsonValue& operator=(JsonValue&&);

    /**
     * \brief Encode this object and all its children to a JSON formatted string
     * \param aPrettyPrint Set to make the string human readable
     * \param aForceToUCS2 Set to use UCS2 codepoints for all characters above ASCII.
     * \return JSON formatted string
     */
    std::string Encode(bool aPrettyPrint = false, bool aForceToUCS2 = false) const;

    /**
     * \brief Decode a string into a JsonValue object
     * \param aJson JSON formatted string
     * \return JsonValue object
     */
    static JsonValue Decode(std::string_view aJson);

    /**
     * \brief Get the type of the value content
     * \return JsonTypes
     */
    JsonTypes GetJsonType() const;

    /**
     * \brief Get the type as a string. Useful for error logging.
     * \return string
     */
    static std::string GetJsonTypeAsString(JsonTypes aType);

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
    bool IsArray() const { return (GetJsonType() == JsonTypes::Array); }
    /**
     * \brief Check if value content is an object
     * \return True if content is an object
     */
    bool IsObject() const { return (GetJsonType() == JsonTypes::Object); }

    /**
     * \brief Index operators for object members
     *
     * \param aKey Name of object member
     * \return Reference to value
     */
    JsonValue& operator[](std::string_view aKey);
    JsonValue& operator[](const std::string& arKey) { return (*this)[std::string_view(arKey)]; }
    JsonValue& operator[](const char* apKey)  { return (*this)[std::string_view(apKey)]; }

    const JsonValue& operator[](std::string_view aKey) const;
    const JsonValue& operator[](const std::string& arKey) const { return (*this)[std::string_view(arKey)]; }
    const JsonValue& operator[](const char* apKey) const { return (*this)[std::string_view(apKey)]; }

    /**
     * \brief Index operators for array members
     *
     * \param aIndex Index number
     * \return Reference to value
     */
    JsonValue& operator[](std::size_t aIndex);
    JsonValue& operator[](int aIndex) { return (*this)[static_cast<std::size_t>(aIndex)]; }
//    JsonValue& operator[](unsigned int aIndex) { return (*this)[static_cast<std::size_t>(aIndex)]; }

    const JsonValue& operator[](std::size_t aIndex) const;
    const JsonValue& operator[](int aIndex) const { return (*this)[static_cast<std::size_t>(aIndex)]; }
//    const JsonValue& operator[](unsigned int aIndex) const { return (*this)[static_cast<std::size_t>(aIndex)]; }

    /**
     * \brief Get the number of element in a object or array
     * \return unsigned int Number of elements
     */
    std::size_t GetCount() const;

    /**
     * \brief Get a list of member names if this is a json object.
     *
     * \return Vector of strings
     */
    std::vector<std::string> GetMemberNames() const;

    /**
     * \brief Check if an object has a member with the specified name
     * \param aKey
     * \return
     */
    bool MemberExists(std::string_view aKey) const;

    /**
     * \brief Add a new element to the array
     * \param aValue JsonValue to append to array
     * \return Reference to this
     */
    JsonValue& Add(JsonValue aValue);

    /**
     * \brief Ass new member element to the object
     * \param aKey Name of member to add
     * \param aValue JsonValue to assign to member
     * \return Reference to this
     */
    JsonValue& Add(std::string_view aKey, JsonValue aValue);

    /**
     * \brief Remove a specified element from the array.
     * \param aIndex Integer index of element to remove
     * \return Reference to this
     */
    JsonValue& Remove(int aIndex);

    /**
     * \brief Remove a named element from an object
     * \param aKey String name of member to remove
     * \return Reference to this
     */
    JsonValue& Remove(std::string_view aKey);

    /**
     * \brief Clear this object. It will be a JsonValue Null object afterwards.
     */
    void Clear() override;

    /**
     * \brief Deep compare two JsonValue objects. Type and content must match.
     * \param arOther
     * \return True if equal
     */
    bool operator==(const JsonValue &arOther) const;

    /**
     * \brief Deep compare two JsonValue objects. Type and content must match.
     * \param arOther
     * \return True if NOT equal
     */
    bool operator!=(const JsonValue &arOther) const
    {
        return !((*this) == arOther);
    }


protected:
    class PrintFormat {
    public:
        PrintFormat() : indent(0) {}
        PrintFormat(unsigned int aIndent, const char* aNl, const char* aSp) : indent(aIndent), nl(aNl), sp(aSp) {}
        unsigned int indent;
        std::string nl{};
        std::string sp{};
    };

    friend JsonDecoder;
    std::string mName{}; // Name if this value is an object member
    std::vector<JsonValue> mItems{};

    void tryArray() const;
    void tryObject() const;
    void forceObject();
    void forceArray();

    void stringToStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2) const;
    void arrayToStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2) const;
    void objectToStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2) const;
    void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2) const;

    static bool mEncodeToUCS2;
};

std::ostream& operator<< (std::ostream& os, const JsonValue &arValue);

} /* namespace rsp::json */

#endif /* INCLUDE_JSON_JSONVALUE_H_ */
