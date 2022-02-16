/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/json/JsonValue.h>
#include <utils/json/JsonArray.h>
#include <utils/json/JsonObject.h>
#include <logging/Logger.h>
#include <utils/StrUtils.h>
#include <utils/json/JsonExceptions.h>


using namespace rsp::utils::json;


JsonValue::~JsonValue()
{
}


JsonString JsonValue::Encode(bool aPrettyPrint)
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

    return JsonString(result.str());
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




