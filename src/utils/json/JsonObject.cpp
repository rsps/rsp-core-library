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

using namespace rsp::utils::json;

JsonObject::JsonObject()
{
    mType = Types::Pointer;
    mPointer = static_cast<uintptr_t>(JsonTypes::Object);
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
    return (mData.find(aName) != mData.end());
}

JsonValue& JsonObject::operator [](const char *apName)
{
    return (*this)[std::string(apName)];
}

JsonValue& JsonObject::operator [](const std::string &aName)
{
    try {
        return *(mData[aName]);
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

    DLOG("JsonObject::Add(): \"" << aName << "\": (" << apValue->Encode() << ")");
    mData[aName] = apValue;
    return *this;
}

JsonObject& JsonObject::Remove(const std::string &aName)
{
    auto it = mData.find(aName);
    if (it != mData.end()) {
        mData.erase(it);
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



