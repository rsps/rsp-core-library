/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#include <cstdlib>
#include <algorithm>
#include <memory>
#include <logging/Logger.h>
#include <utils/Json.h>
#include <utils/StrUtils.h>
#include <utils/ExceptionHelper.h>

namespace rsp::utils {

JsonValue* JsonValue::Decode(UTF8String &arJson)
{
    JsonValue* result = nullptr;

    auto it = whitespace(arJson.begin());
    if (it != arJson.end()) {
        switch (*it) {
            case '{':
                DLOG("Object detected: " << arJson);
                result = new JsonObject(arJson);
                DLOG("Result Type: " << result->typeToText() << ", Count: " << result->AsObject().GetCount());
                break;

            case '[':
                DLOG("Array detected: " << arJson);
                result = new JsonArray(arJson);
                DLOG("Result Type: " << result->typeToText() << ", Count: " << result->AsArray().GetCount());
                break;

            case '"':
                DLOG("String detected: " << arJson );
                result = new JsonValue(parseString(arJson));
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '-':
                DLOG("Number detected: " << arJson );
                result = new JsonValue();
                parseNumber(arJson, result);
                break;

            case 't':
                DLOG("true detected: " << arJson );
                if (StrUtils::StartsWith(arJson, "true")) {
                    arJson.erase(0, 4);
                    result = new JsonValue(true);
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonParseError);
                }
                break;

            case 'f':
                DLOG("false detected: " << arJson );
                if (StrUtils::StartsWith(arJson, "false")) {
                    arJson.erase(0, 5);
                    result = new JsonValue(false);
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonParseError);
                }
                break;

            case 'n':
                DLOG("null detected: " << arJson );
                if (StrUtils::StartsWith(arJson, "null")) {
                    arJson.erase(0, 4);
                    result = new JsonValue();
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonParseError);
                }
                break;

            default:
                DLOG("Illegal start character: " << std::hex << static_cast<int>(*it) << ", input: " << arJson );
                THROW_WITH_BACKTRACE(EJsonParseError);
                break;
        }
    }

    return result;
}

void JsonValue::jWalk(const char aToken1, const char aToken2, UTF8String &arJson, UTF8String &arResult)
{
    auto it = whitespace(arJson.begin());

    if (*it == 0) {
        return;
    }

    if (*it == aToken1) {
        it++;
    }
    int stack = 0;

    while (*it) {
        if (*it == aToken2) {
            if (stack == 0) {
                arJson.erase(arJson.begin(), it);
                return;
            }
            else {
                stack--;
            }
        }
        else if (*it == aToken1) {
            stack++;
        }
        else if (*it == '\\') {
            arResult += *it;
            it++;
        }
        arResult += *it;
        it++;
    }

    THROW_WITH_BACKTRACE(EJsonFormatError);
}

std::string::iterator JsonValue::whitespace(std::string::iterator aIt)
{
    int max_chars = 4096; // We should limit ourself in case of bad pointer.

    do {
        switch (*aIt) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                aIt++;
                break;

            default:
                return aIt;
                break;
        }

        if (--max_chars == 0) {
            THROW_WITH_BACKTRACE(EJsonFormatError);
        }
    }
    while (*aIt);

    return aIt;
}

UTF8String JsonValue::parseString(UTF8String &arJson)
{
    UTF8String s;
    UTF8String result;

    JsonValue::jWalk('"', '"', arJson, s);
    unsigned int c = s.length();
    unsigned int i = 0;
    while (i < c) {
        if (s[i] == '\\') {
            i++;
            switch (s[i]) {
                case 'u':
                {
                    int u = std::stoi(s.substr(i+1, 4), nullptr, 16);
                    if (u > 256) {
                        throw EJsonUnicodeError();
                    }
                    result += static_cast<char>(u);
                    i += 4;
                    break;
                }

                case '"':
                case '\\':
                case '/':
                    result += s[i];
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;

                default:
                    THROW_WITH_BACKTRACE(EJsonParseError);
                    break;
            }
        }
        else {
            result += s[i];
        }
        i++;
    }

    return result;
}

void JsonValue::parseNumber(UTF8String &arJson, JsonValue* apResult)
{
    bool is_float = false;
    bool is_negative = false;
    auto it = whitespace(arJson.begin());
    std::string result;
    int i = 0;
    while (it != arJson.end()) {
        char c = *it;
        int skip = 0;
        std::cout << "c: " << c << ", i: " << i << std::endl;
        switch (i) {
            case 0:
                if (c == '-') {
                    is_negative = true;
                    i = 1;
                }
                else {
                    skip = 1;
                }
                break;

            case 1:
                if (c == '0') {
                    i = 3;
                }
                else if (c >= '0' && c <= '9') {
                    i = 2;
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonNumberError);
                }
                break;

            case 2:
                if (!(c >= '0' && c <= '9')) {
                    skip = 3;
                }
                break;

            case 3:
                if (c == '.') {
                    is_float = true;
                    i = 4;
                }
                else {
                    skip = 6;
                }
                break;

            case 4:
                if (!(c >= '0' && c <= '9')) {
                    THROW_WITH_BACKTRACE(EJsonNumberError);
                }
                else {
                    i = 5;
                }
                break;

            case 5:
                if (!(c >= '0' && c <= '9')) {
                    skip = 6;
                }
                break;

            case 6:
                if (c == 'e' || c == 'E') {
                    is_float = true;
                    i = 7;
                }
                else {
                    skip = 10;
                }
                break;

            case 7:
                if (c == '+' || c == '-') {
                    i = 8;
                }
                else {
                    skip = 8;
                }
                break;

            case 8:
                if (!(c >= '0' && c <= '9')) {
                    THROW_WITH_BACKTRACE(EJsonNumberError);
                }
                else {
                    i = 9;
                }
                break;

            case 9:
                if (!(c >= '0' && c <= '9')) {
                    skip = 10;
                }
                break;

            case 10: // delimiter
                if (c == ',' || it == arJson.end()) {
                    skip = 100;
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonNumberError);
                }
                break;

            default:
                break;
        }
        if (i < 100) {
            if (skip > 0) {
                i = skip;
            }
            else {
                it++;
                result += c;
            }
        }
        else {
            break;
        }
    }

    if (is_float) {
        *apResult = std::strtod(result.c_str(), nullptr);
    }
    else if (is_negative) {
        *apResult = static_cast<std::int64_t>(std::strtoll(result.c_str(), nullptr, 10));
    }
    else {
        *apResult = static_cast<std::int64_t>(std::strtoull(result.c_str(), nullptr, 10));
    }
}

JsonValue::~JsonValue()
{
}


UTF8String JsonValue::Encode(bool aPrettyPrint)
{
    std::stringstream result;

    if (aPrettyPrint) {
        PrintFormat pf(4, "\n", " ");
        toStringStream(result, pf, 0);
    }
    else {
        PrintFormat pf;
        toStringStream(result, pf, 0);
    }

    return UTF8String(result.str());
}


JsonArray& JsonValue::AsArray() const
{
    if (mType == Types::Pointer && mPointer == static_cast<uintptr_t>(JsonTypes::Array)) {
        return *dynamic_cast<JsonArray*>(const_cast<JsonValue*>(this));
    }
    else {
        THROW_WITH_BACKTRACE1(EJsonTypeError, "JsonValue of type " + jsonTypeToText() + " cannot be converted to Array");
    }
}

JsonObject& JsonValue::AsObject() const
{
    if (mType == Types::Pointer && mPointer == static_cast<uintptr_t>(JsonTypes::Object)) {
        return *dynamic_cast<JsonObject*>(const_cast<JsonValue*>(this));
    }
    else {
        THROW_WITH_BACKTRACE1(EJsonTypeError, "JsonValue of type " + jsonTypeToText() + " cannot be converted to Object");
    }
}

void JsonValue::toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel)
{
    std::string in(arPf.indent * aLevel, ' ');

    if (mType == Types::String) {
        std::string s = AsString();
        for (std::size_t i = 0 ; i < s.length() ; i++) {
            auto c = s[i];
            switch (c) {
                case '\"':
                    s.replace(i, 1, "\\\"");
                    break;
                case '\\':
                    s.replace(i, 1, "\\\\");
                    break;
                case '\b':
                    s.replace(i, 1, "\\b");
                    break;
                case '\f':
                    s.replace(i, 1, "\\f");
                    break;
                case '\n':
                    s.replace(i, 1, "\\n");
                    break;
                case '\r':
                    s.replace(i, 1, "\\r");
                    break;
                case '\t':
                    s.replace(i, 1, "\\t");
                    break;
                default:
                    if (static_cast<std::uint8_t>(c) > 127) {
                        char buf[12];
                        sprintf(buf, "\\u%04x", static_cast<int>(c));
                        s.replace(i, 1, buf);
                    }
                    break;
            }
        }
        arResult << "\"" << s << "\"";
    }
    else {
        arResult << AsString();
    }
}

std::string JsonValue::jsonTypeToText() const
{
    switch (GetJsonType()) {
        case JsonTypes::Array: return "Array";
        case JsonTypes::Bool: return "Bool";
        default:
        case JsonTypes::Null: return "Null";
        case JsonTypes::Number: return "Number";
        case JsonTypes::Object: return "Object";
        case JsonTypes::String: return "String";
    }
}

JsonTypes JsonValue::GetJsonType() const
{
    switch (mType) {
        default:
        case Types::Null:
            return JsonTypes::Null;
        case Types::Bool:
            return JsonTypes::Bool;
        case Types::Int:
        case Types::Int64:
        case Types::Uint32:
        case Types::Float:
        case Types::Double:
            return JsonTypes::Number;
        case Types::Pointer:
            if (mPointer == static_cast<uintptr_t>(JsonTypes::Object)) {
                return JsonTypes::Object;
            }
            if (mPointer == static_cast<uintptr_t>(JsonTypes::Array)) {
                return JsonTypes::Array;
            }
            THROW_WITH_BACKTRACE1(EJsonTypeError, "Variant of type " + typeToText() + " cannot be converted to JsonTypes Object or Array");
            break;
    }
}


JsonArray::JsonArray()
{
    mType = Types::Pointer;
    mPointer = static_cast<uintptr_t>(JsonTypes::Array);
}

JsonArray::JsonArray(UTF8String &arJson)
{
    mType = Types::Pointer;
    mPointer = static_cast<uintptr_t>(JsonTypes::Array);

    UTF8String s;

    JsonValue::jWalk('[', ']', arJson, s);

    while (s.length() > 0) {
        Add(JsonValue::Decode(s));
        StrUtils::LeftTrim(s);
        if (s.length() > 0 && s[0] == ',') {
            s.erase(0, 1);
        }
    }
}

JsonArray::~JsonArray()
{
    Clear();
}

std::size_t JsonArray::GetCount() const
{
    return mData.size();
}

JsonValue& JsonArray::operator [](unsigned int aIndex)
{
    return *(mData[aIndex]);
}

JsonArray& JsonArray::Add(JsonValue* apValue)
{
    if (apValue == nullptr) {
        return *this;
    }
    DLOG("JsonArray::Add(): " << apValue->Encode());
    mData.push_back(apValue);

    return *this;
}

JsonArray& JsonArray::Remove(int aIndex)
{
    mData.erase(mData.begin() + aIndex);
    return *this;
}

void JsonArray::Clear()
{
    for (auto el : mData) {
        delete el;
    }
    mData.clear();

    mType = Types::Null;
}

void JsonArray::toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel)
{
    std::string in(arPf.indent * (aLevel + 1), ' ');
    std::string c = ",";

    arResult << "[" << arPf.nl;

    int rest = mData.size();
    for (auto el : mData) {
       arResult << in;
       el->toStringStream(arResult, arPf, aLevel+1);
       if (--rest == 0) {
           c = "";
       }
       arResult << c << arPf.nl;
    }
    std::string in1(arPf.indent * aLevel, ' ');
    arResult << in1 << "]";
}


JsonObject::JsonObject()
{
    mType = Types::Pointer;
    mPointer = static_cast<uintptr_t>(JsonTypes::Object);
}

JsonObject::JsonObject(UTF8String &arJson)
{
    mType = Types::Pointer;
    mPointer = static_cast<uintptr_t>(JsonTypes::Object);

    UTF8String s;
    std::string name;

    JsonValue::jWalk('{', '}', arJson, s);

    while (s.length() > 0) {
        name = JsonValue::parseString(s);
        StrUtils::LeftTrim(s);
        if (s[0] != ':') {
            THROW_WITH_BACKTRACE(EJsonFormatError);
        }
        s.erase(0, 1);
        Add(name, JsonValue::Decode(s));
        StrUtils::LeftTrim(s);
        if (s.length() > 0 && s[0] == ',') {
            s.erase(0, 1);
        }
    }
}

JsonObject::~JsonObject()
{
    Clear();
}

std::size_t JsonObject::GetCount() const
{
    return mData.size();
}

bool JsonObject::MemberExists(const std::string &aName) const
{
    return (mIndex.find(aName) != mIndex.end());
}

JsonValue& JsonObject::operator [](const char *apName)
{
    return (*this)[std::string(apName)];
}

JsonValue& JsonObject::operator [](const std::string &aName)
{
    try {
        return *(mData[mIndex.at(aName)].second);
    }
    catch (const std::exception &e) {
        THROW_WITH_BACKTRACE1(EJsonException, "JsonObject: Member \"" + aName + "\" not found. <- " + e.what());
    }
}

JsonObject& JsonObject::Add(const std::string &aName, JsonValue* apValue)
{
    if (apValue == nullptr) {
        return *this;
    }

    DLOG("JsonObject::Add(): \"" << aName << "\": " << apValue->Encode());
    mData.push_back({aName, apValue});
    mIndex[aName] = mData.size() - 1;
    return *this;
}

JsonObject& JsonObject::Remove(const std::string &aName)
{
    auto it = mIndex.find(aName);
    if (it != mIndex.end()) {
        unsigned int idx = it->second;
        mIndex.erase(it);
        mData.erase(mData.begin() + idx);
    }
    return *this;
}

void JsonObject::Clear()
{
    mIndex.clear();

    for (auto el : mData) {
        delete el.second;
    }
    mData.clear();

    mType = Types::Null;
}

void JsonObject::toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel)
{
    std::string in(arPf.indent * (aLevel+1), ' ');
    std::string c = ",";

    arResult << "{" << arPf.nl;

    int rest = mData.size();
    for (auto el : mData) {
       arResult << in << "\"" << el.first << "\":" << arPf.sp;
       el.second->toStringStream(arResult, arPf, aLevel+1);
       if (--rest == 0) {
           c = "";
       }
       arResult << c << arPf.nl;
    }
    std::string in1(arPf.indent * aLevel, ' ');
    arResult << in1 << "}";
}

} /* namespace rsp::utils */
