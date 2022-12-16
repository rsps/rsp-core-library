/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <json/JsonDecoder.h>
#include <json/JsonExceptions.h>
#include <json/JsonValue.h>
#include <iomanip>
#include <logging/Logger.h>
#include <utils/StrUtils.h>

using namespace rsp::logging;

namespace rsp::json {

//#define JLOG(a) DLOG(a)
#define JLOG(a)


std::ostream& operator<<(std::ostream& os, JsonTypes aType)
{
    os << JsonValue::GetJsonTypeAsString(aType);
    return os;
}

std::ostream& operator<< (std::ostream& os, const JsonValue &arValue)
{
    os << arValue.Encode();
    return os;
}


JsonValue::JsonValue(JsonTypes aType)
    : Variant()
{
    JLOG("JsonValue type constructor: " << aType);
    switch (aType) {
        case JsonTypes::Array:
        case JsonTypes::Object:
            mType = Types::Pointer;
            mPointer = static_cast<uintptr_t>(aType);
            break;

        default:
            break;
    }
}

JsonValue::JsonValue(const JsonValue& arOther)
    : Variant(arOther),
      mName(arOther.mName),
      mItems(arOther.mItems)
{
    JLOG("JsonValue copy constructor");
}

JsonValue::JsonValue(JsonValue&& arOther)
    : Variant(std::move(arOther)),
      mName(std::move(arOther.mName)),
      mItems(std::move(arOther.mItems))
{
    JLOG("JsonValue move constructor");
}

JsonValue::~JsonValue()
{
}

JsonValue& JsonValue::operator=(const JsonValue& arOther)
{
    if (&arOther != this) {
        Variant::operator=(arOther);
        mName = arOther.mName;
        mItems = arOther.mItems;
        JLOG("JsonValue copy assignment");
    }
    return *this;
}


JsonValue& JsonValue::operator=(JsonValue&& arOther)
{
    if (&arOther != this) {
        Variant::operator=(std::move(arOther));
        mName = std::move(arOther.mName);
        mItems = std::move(arOther.mItems);
        JLOG("JsonValue move assignment");
    }
    return *this;
}


std::string JsonValue::Encode(bool aPrettyPrint, bool aForceToUCS2) const
{
//    JLOG("Encode(" << aPrettyPrint << ", " << aForceToUCS2 << ") on " << *this);
    std::stringstream result;

    if (aPrettyPrint) {
        PrintFormat pf(4, "\n", " ");
        toStringStream(result, pf, 0, aForceToUCS2);
    }
    else {
        PrintFormat pf;
        toStringStream(result, pf, 0, aForceToUCS2);
    }

    return result.str();
}

JsonValue JsonValue::Decode(std::string_view aJson)
{
    JsonDecoder js(aJson);

    JsonValue result;
    result = js.GetValue();
    return result;
}

JsonValue& JsonValue::operator [](std::string_view aKey)
{
    JLOG("Access member " << aKey);
    forceObject();
    for (JsonValue &v : mItems) {
        if (v.mName == aKey) {
            return v;
        }
    }
    THROW_WITH_BACKTRACE1(EMemberNotExisting, std::string(aKey));
}

const JsonValue& JsonValue::operator [](std::string_view aKey) const
{
    JLOG("Getting member " << aKey);
    tryObject();
    for (const JsonValue &v : mItems) {
        if (v.mName == aKey) {
            return v;
        }
    }
    THROW_WITH_BACKTRACE1(EMemberNotExisting, std::string(aKey));
}

JsonValue& JsonValue::operator [](std::size_t aIndex)
{
    JLOG("Access item " << aIndex);
    forceArray();
    return mItems.at(aIndex);
}

const JsonValue& JsonValue::operator [](std::size_t aIndex) const
{
    JLOG("Getting item " << aIndex);
    tryArray();
    return mItems.at(aIndex);
}

std::size_t JsonValue::GetCount() const
{
    switch (mType) {
        default:
            return 0;

        case Types::Pointer:
            if ((mPointer == static_cast<uintptr_t>(JsonTypes::Object)) || (mPointer == static_cast<uintptr_t>(JsonTypes::Array))) {
                return mItems.size();
            }
            THROW_WITH_BACKTRACE1(EJsonTypeError, "Variant of type " + typeToText() + " must be of JsonType Object or Array");
            break;
    }
}

std::vector<std::string> JsonValue::GetMemberNames() const
{
    tryObject();
    std::vector<std::string> result;
    for (const JsonValue &jv : mItems) {
        result.push_back(jv.mName);
    }
    return result;
}

bool JsonValue::MemberExists(std::string_view aKey) const
{
    tryObject();
    for (const JsonValue &v : mItems) {
        if (v.mName == aKey) {
            return true;
        }
    }
    return false;
}

JsonValue& JsonValue::Add(JsonValue aValue)
{
    forceArray();
    Logger::GetDefault().Info() << "JsonArray::Add(): " << aValue.Encode() << std::endl;
    mItems.emplace_back(aValue);
    return *this;
}

JsonValue& JsonValue::Add(std::string_view aKey, JsonValue aValue)
{
    forceObject();
    Logger::GetDefault().Info() << "JsonObject::Add(): \"" << aKey << "\": " << aValue.Encode() << std::endl;
    aValue.mName = aKey;
    mItems.push_back(aValue);
    return *this;
}

JsonValue& JsonValue::Remove(int aIndex)
{
    tryArray();
    mItems.erase(mItems.begin() + aIndex);
    return *this;
}

JsonValue& JsonValue::Remove(std::string_view aKey)
{
    tryObject();
    auto it = std::find_if(mItems.begin(), mItems.end(), [&aKey](const JsonValue& arValue) {
        if (arValue.mName == aKey) {
            return true;
        }
        return false;
    });
    mItems.erase(it);
    return *this;
}

void JsonValue::Clear()
{
    mName.clear();
    mItems.clear();
    mType = Types::Null;
}


void JsonValue::tryArray() const
{
    if (!IsArray()) {
        THROW_WITH_BACKTRACE1(EJsonTypeError, "JsonValue of type " + GetJsonTypeAsString(GetJsonType()) + " cannot be converted to Array");
    }
}

void JsonValue::tryObject() const
{
    if (!IsObject()) {
        THROW_WITH_BACKTRACE1(EJsonTypeError, "JsonValue of type " + GetJsonTypeAsString(GetJsonType()) + " cannot be converted to Object");
    }
}

void JsonValue::forceArray()
{
    if (mType == Types::Null) {
        mType = Types::Pointer;
        mPointer = static_cast<uintptr_t>(JsonTypes::Array);
        return;
    }
    tryArray();
}

void JsonValue::forceObject()
{
    if (mType == Types::Null) {
        mType = Types::Pointer;
        mPointer = static_cast<uintptr_t>(JsonTypes::Object);
        return;
    }
    tryObject();
}


void JsonValue::toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2) const
{
    switch(GetJsonType()) {
        case JsonTypes::String:
            stringToStringStream(arResult, arPf, aLevel, aForceToUCS2);
            break;

        case JsonTypes::Array:
            arrayToStringStream(arResult, arPf, aLevel, aForceToUCS2);
            break;

        case JsonTypes::Object:
            objectToStringStream(arResult, arPf, aLevel, aForceToUCS2);
            break;

        default:
            arResult << AsString();
        break;
    }
}

std::string JsonValue::GetJsonTypeAsString(JsonTypes aType)
{
    switch (aType) {
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
        case Types::Null:
            return JsonTypes::Null;
        case Types::Bool:
            return JsonTypes::Bool;
        case Types::Int:
        case Types::Int64:
        case Types::Uint64:
        case Types::Uint32:
        case Types::Uint16:
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
            break;
        case Types::String:
            return JsonTypes::String;
        default:
            THROW_WITH_BACKTRACE1(EJsonTypeError, "Variant of type " + typeToText() + " is not a valid JSON type");
    }
    return JsonTypes::Null; // We should never get here...
}

void JsonValue::stringToStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel,
    bool aForceToUCS2) const
{
    std::string s = AsString();
    std::size_t i = 0;
    while (i < s.length()) {
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
                if (aForceToUCS2 && static_cast<uint8_t>(c) > 127) {
                    int v = 0;
                    char buf[12];
                    switch (static_cast<uint8_t>(c) & 0xE0) {
                        case 0xE0:
                            v =   ((static_cast<int>(s[i]) & 0x0F) << 12)
                                + ((static_cast<int>(s[i+1]) & 0x3F) << 6)
                                + (static_cast<int>(s[i+2]) & 0x3F);
                            sprintf(buf, "\\u%04x", v);
                            s.replace(i, 3, buf);
                            i += 4;
                            break;

                        case 0xC0:
                            v =   ((static_cast<int>(s[i]) & 0x1F) << 6)
                                + (static_cast<int>(s[i+1]) & 0x3F);
                            sprintf(buf, "\\u%04x", v);
                            s.replace(i, 2, buf);
                            i += 4;
                            break;

                        default:
                            THROW_WITH_BACKTRACE1(EJsonParseError, "JsonValue of type string has illegal character: " + c);
                            break;
                    }
                }
                break;
        }
        i++;
    }
    arResult << "\"" << s << "\"";
//        DLOG("toStringSteam: " << s);
//        for (auto c : s) {
//            DLOG("toStringSteam: " << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(c) << std::dec << " " << c);
//        }
}

void JsonValue::arrayToStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel,
    bool aForceToUCS2) const
{
    std::string in(static_cast<std::string::size_type>(arPf.indent) * (aLevel + 1), ' ');
    std::string c = ",";

    arResult << "[" << arPf.nl;

    auto rest = mItems.size();
    for (const JsonValue& el : mItems) {
       arResult << in;
       el.toStringStream(arResult, arPf, aLevel+1, aForceToUCS2);
       if (--rest == 0) {
           c = "";
       }
       arResult << c << arPf.nl;
    }
    std::string in1(static_cast<std::string::size_type>(arPf.indent) * aLevel, ' ');
    arResult << in1 << "]";
}

void JsonValue::objectToStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel,
    bool aForceToUCS2) const
{
    std::string in(static_cast<std::string::size_type>(arPf.indent) * (aLevel+1), ' ');
    std::string c = ",";

    arResult << "{" << arPf.nl;

    JLOG("Members:");
    for(const JsonValue &value : mItems) {
        JLOG("  " << value.mName << ": " << value.AsString());
    }

    auto rest = mItems.size();
    for (const JsonValue &value : mItems) {
       arResult << in << "\"" << value.mName << "\":" << arPf.sp;

       value.toStringStream(arResult, arPf, aLevel+1, aForceToUCS2);
       if (--rest == 0) {
           c = "";
       }
       arResult << c << arPf.nl;
    }
    std::string in1(static_cast<std::string::size_type>(arPf.indent) * aLevel, ' ');
    arResult << in1 << "}";
}

bool JsonValue::operator ==(const JsonValue &arOther) const
{
    if (this == &arOther) {
        return true;
    }
    if (GetJsonType() != arOther.GetJsonType()) {
        return false;
    }
    switch(GetJsonType()) {
        case JsonTypes::Null:
            return (arOther.GetJsonType() == JsonTypes::Null);

        case JsonTypes::Bool:
            return (AsBool() == arOther.AsBool());

        case JsonTypes::Number:
            return (AsDouble() == arOther.AsDouble());

        case JsonTypes::String:
            return (AsString() == arOther.AsString());

        case JsonTypes::Object:
            for (const JsonValue &jv : mItems) {
                if (!(jv == arOther[jv.mName])) {
                    return false;
                }
            }
            return true;

        case JsonTypes::Array:
            int i = 0;
            for (const JsonValue &jv : mItems) {
                if (!(jv == arOther[i])) {
                    return false;
                }
            }
            return true;
    }
    return false;
}

} // namespace rsp::json

