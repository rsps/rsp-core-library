/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_JSON_H_
#define INCLUDE_UTILS_JSON_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>
#include "CoreException.h"
#include "Variant.h"

namespace rsp::utils {

class EJsonException : public CoreException {
public:
    explicit EJsonException(const char *apMsg) : CoreException(apMsg) {}
    EJsonException(const std::string &aMsg) : CoreException(aMsg) {}
};

class EJsonParseError : public EJsonException {
public:
    explicit EJsonParseError() : EJsonException("Json Parse Error") {}
};

class EJsonFormatError : public EJsonException {
public:
    explicit EJsonFormatError() : EJsonException("Json Format Error") {}
};

class EJsonNumberError : public EJsonException {
public:
    explicit EJsonNumberError() : EJsonException("Json Number Error") {}
};

class EJsonTypeError : public EJsonException {
public:
    explicit EJsonTypeError(const std::string &aMsg) : EJsonException(aMsg) {}
};

class EJsonUnicodeError : public EJsonException {
public:
    explicit EJsonUnicodeError() : EJsonException("This Json implementation does not support unicode values greater than U+00FF") {}
};


typedef std::string UTF8String;

class JsonObject;
class JsonArray;

enum class JsonTypes : unsigned int { Null, Bool, Number, String, Object, Array };

class JsonValue : public Variant
{
public:
    static JsonValue* Decode(const char* apJson) { UTF8String js(apJson); return Decode(js); }
    static JsonValue* Decode(const UTF8String &arJson) { UTF8String js(arJson); return Decode(js); }
    static JsonValue* Decode(UTF8String &arJson);
    UTF8String Encode(bool aPrettyPrint = false);

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

    static void jWalk(const char aToken1, const char aToken2, UTF8String &arJson, UTF8String &arResult);
    static std::string::iterator whitespace(std::string::iterator aIt);
    static UTF8String parseString(UTF8String &arJson);
    static void parseNumber(UTF8String &arJson, JsonValue* apResult);
};

class JsonArray : public JsonValue
{
public:
    JsonArray();
    JsonArray(UTF8String &arJson);
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

class JsonObject : public JsonValue
{
public:
    JsonObject();
    JsonObject(UTF8String &arJson);
    ~JsonObject() override;

    std::size_t GetCount() const;
    bool MemberExists(const std::string &aName) const;

    JsonValue& operator[](const char *apName);
    JsonValue& operator[](const std::string &aName);

    JsonObject& Add(const std::string &aName, JsonValue* apValue);
    JsonObject& Remove(const std::string &aName);
    void Clear() override;

protected:
    std::vector<std::pair<std::string, JsonValue*>> mData{};
    std::map<const std::string, unsigned int> mIndex{};

    void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel) override;
};

template <class T>
class Json : public JsonValue
{
    T Get() { return static_cast<T>(*this); }
    void Set(T aValue) { (*this) = aValue; }
};

} /* namespace rsp */

#endif /* INCLUDE_UTILS_JSON_H_ */
