/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <json/JsonExceptions.h>
#include <json/JsonObject.h>
#include <logging/Logger.h>

namespace rsp::json {

//#define JLOG(a) DLOG(a)
#define JLOG(a)

JsonObject::JsonObject()
    : JsonValue()
{
    mType = Types::Pointer;
    mPointer = static_cast<uintptr_t>(JsonTypes::Object);
}

JsonObject::JsonObject(const JsonObject &arOther)
    : JsonValue(static_cast<const JsonValue&>(arOther))
{
    JLOG("JsonObject copy constructor");
    for(auto key : arOther.mData.GetOrderList()) {
        mData[key] = arOther.mData.at(key)->clone();
    }
}

JsonObject::JsonObject(JsonObject &&arOther)
    : JsonValue(static_cast<JsonValue&&>(arOther))
{
    JLOG("JsonObject move constructor");
    mData = std::move(arOther.mData);
    arOther.mData.clear();
    arOther.Clear();
}

JsonObject::~JsonObject()
{
    Clear();
}

JsonObject& JsonObject::operator=(const JsonObject &arOther)
{
    JLOG("JsonObject copy assignment");
    JsonValue::operator=(static_cast<const JsonValue&>(arOther));
    mData.clear();
    for(auto key : arOther.mData.GetOrderList()) {
        mData[key] = arOther.mData.at(key)->clone();
    }
    return *this;
}

JsonValue* JsonObject::clone() const
{
    JLOG("JsonObject::clone");
    return new JsonObject(*this);
}

JsonObject& JsonObject::operator=(JsonObject &&arOther)
{
    JLOG("JsonObject move assignment");
    JsonValue::operator=(static_cast<JsonValue&&>(arOther));
    mData = std::move(arOther.mData);
    arOther.mData.clear();
    arOther.Clear();
    return *this;
}

std::size_t JsonObject::GetCount() const
{
    return mData.size();
}

bool JsonObject::MemberExists(const std::string &arName) const
{
    return (mData.GetMap().count(arName) > 0);
}

JsonValue& JsonObject::operator [](const char *apName)
{
    return (*this)[std::string(apName)];
}

JsonValue& JsonObject::operator [](const std::string &arName)
{
    try {
        return *(mData.at(arName));
    }
    catch (const std::exception &e) {
        THROW_WITH_BACKTRACE1(EJsonException, "JsonObject: Member \"" + arName + "\" not found. <- " + e.what());
    }
}

JsonObject& JsonObject::Add(const std::string &arName, JsonValue* apValue)
{
    if (apValue == nullptr) {
        return *this;
    }

    DLOG("JsonObject::Add(): \"" << arName << "\": " << apValue->Encode());
    mData.Remove(arName);
    mData[arName] = apValue;
    return *this;
}

JsonObject& JsonObject::Remove(const std::string &arName)
{
    mData.Remove(arName);

    return *this;
}

void JsonObject::Clear()
{
    for (auto el : mData.GetMap()) {
        delete el.second;
    }
    mData.clear();

    mType = Types::Null;
}

void JsonObject::toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2)
{
    std::string in(static_cast<std::string::size_type>(arPf.indent) * (aLevel+1), ' ');
    std::string c = ",";

    arResult << "{" << arPf.nl;

    int rest = mData.size();
    for (auto key : mData.GetOrderList()) {
       auto value = mData.at(key.get());
       arResult << in << "\"" << key.get() << "\":" << arPf.sp;
       value->toStringStream(arResult, arPf, aLevel+1, aForceToUCS2);
       if (--rest == 0) {
           c = "";
       }
       arResult << c << arPf.nl;
    }
    std::string in1(static_cast<std::string::size_type>(arPf.indent) * aLevel, ' ');
    arResult << in1 << "}";
}

} // namespace rsp::json
