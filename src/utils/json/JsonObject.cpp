/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <utils/json/JsonObject.h>
#include <utils/json/JsonExceptions.h>
#include <logging/Logger.h>

namespace rsp::utils::json {

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
    mKeyNames = arOther.mKeyNames;

    for(auto el : arOther.mData) {
        mData[el.first] = el.second->clone();
    }
}

JsonObject::JsonObject(JsonObject &&arOther)
    : JsonValue(static_cast<JsonValue&&>(arOther))
{
    JLOG("JsonObject move constructor");
    mKeyNames = std::move(arOther.mKeyNames);
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
    mKeyNames = arOther.mKeyNames;
    mData.clear();
    for(auto el : arOther.mData) {
        mData[el.first] = el.second->clone();
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
    mKeyNames = std::move(arOther.mKeyNames);
    mData = std::move(arOther.mData);
    arOther.mData.clear();
    arOther.Clear();
    return *this;
}

std::size_t JsonObject::GetCount() const
{
    return mData.size();
}

bool JsonObject::MemberExists(const std::string &aName) const
{
    return (mData.find(aName) != mData.end());
}

JsonValue& JsonObject::operator [](const char *apName)
{
    return (*this)[std::string(apName)];
}

JsonValue& JsonObject::operator [](const std::string &aName)
{
    try {
        return *(mData.at(aName));
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
    mKeyNames.push_back(aName);
    mData[aName] = apValue;
    return *this;
}

JsonObject& JsonObject::Remove(const std::string &aName)
{
    auto it = mData.find(aName);
    if (it != mData.end()) {
        mData.erase(it);
    }

    auto position = std::find(mKeyNames.begin(), mKeyNames.end(), aName);
    if (position != mKeyNames.end()) {
        mKeyNames.erase(position);
    }

    return *this;
}

void JsonObject::Clear()
{
    for (auto el : mData) {
        delete el.second;
    }
    mData.clear();

    mType = Types::Null;
}

void JsonObject::toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2)
{
    std::string in(arPf.indent * (aLevel+1), ' ');
    std::string c = ",";

    arResult << "{" << arPf.nl;

    int rest = mData.size();
    for (auto key : mKeyNames) {
       auto value = mData[key];
       arResult << in << "\"" << key << "\":" << arPf.sp;
       value->toStringStream(arResult, arPf, aLevel+1, aForceToUCS2);
       if (--rest == 0) {
           c = "";
       }
       arResult << c << arPf.nl;
    }
    std::string in1(arPf.indent * aLevel, ' ');
    arResult << in1 << "}";
}

} // namespace rsp::utils::json
