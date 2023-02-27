/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <json/Json.h>
#include <iomanip>
#include <logging/Logger.h>
#include <utils/StrUtils.h>
#include <magic_enum.hpp>

using namespace rsp::logging;
using namespace rsp::utils;

namespace rsp::json {

//#define JLOG(a) DLOG(a)
#define JLOG(a)


std::ostream& operator<<(std::ostream& os, Json::Types aType)
{
    os << magic_enum::enum_name<Json::Types>(aType);
    return os;
}

std::ostream& operator <<(std::ostream &os, const Json &arJson)
{
    return os << arJson.Encode();
}

Json::Json(std::string_view aJson)
    : DynamicData(JsonDecoder(aJson).Decode())
{
}

Json::Json(const DynamicData &arData)
    : DynamicData(arData)
{
}

Json::Json(DynamicData&& arData)
    : DynamicData(arData)
{
}

std::string Json::Encode(const DynamicData &arData, bool aPrettyPrint, bool aForceToUCS2, unsigned int aArrayLineLength)
{
    JsonEncoder je(aPrettyPrint, aArrayLineLength, aForceToUCS2);
    return je.Encode(arData);
}

std::string Json::Encode(bool aPrettyPrint, bool aForceToUCS2, unsigned int aArrayLineLength) const
{
    return Encode(*this, aPrettyPrint, aForceToUCS2, aArrayLineLength);
}

DynamicData Json::Decode(std::string_view aJson)
{
    JsonDecoder js(aJson);
    return js.Decode();
}


std::string Json::GetJsonTypeAsString(Json::Types aType)
{
    return std::string(magic_enum::enum_name<Json::Types>(aType));
}

std::string Json::GetJsonTypeAsString()
{
    return GetJsonTypeAsString(GetJsonType());
}

Json::Types Json::GetJsonType(const DynamicData &arData)
{
    switch (arData.GetType()) {
        case DynamicData::Types::Null:
            return Types::Null;
        case DynamicData::Types::Bool:
            return Types::Bool;
        case DynamicData::Types::Int:
        case DynamicData::Types::Int64:
        case DynamicData::Types::Uint64:
        case DynamicData::Types::Uint32:
        case DynamicData::Types::Uint16:
        case DynamicData::Types::Float:
        case DynamicData::Types::Double:
            return Types::Number;
        case DynamicData::Types::Object:
            return Types::Object;
        case DynamicData::Types::Array:
            return Types::Array;
        case DynamicData::Types::String:
            return Types::String;

        case DynamicData::Types::Pointer:
        default:
            break;
    }
    THROW_WITH_BACKTRACE1(EJsonTypeError, "Variant of type " + arData.TypeToText() + " is not a valid JSON type");
}

Json::Types Json::GetJsonType()
{
    return GetJsonType(*this);
}

} // namespace rsp::json

