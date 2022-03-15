/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/json/JsonArray.h>
#include <logging/Logger.h>

namespace rsp::utils::json {

//#define JLOG(a) DLOG(a)
#define JLOG(a)


JsonArray::JsonArray()
{
    mType = Types::Pointer;
    mPointer = static_cast<uintptr_t>(JsonTypes::Array);
}

JsonArray::JsonArray(const JsonArray &arOther)
    : JsonValue(static_cast<const JsonValue&>(arOther))
{
    JLOG("JsonArray copy constructor");
    for(auto el : arOther.mData) {
        mData.push_back(el->clone());
    }
}

JsonArray::JsonArray(JsonArray &&arOther)
    : JsonValue(static_cast<JsonValue&>(arOther)),
      mData(arOther.mData)
{
    JLOG("JsonArray move constructor");
    arOther.mData.clear();
}

JsonArray::~JsonArray()
{
    Clear();
}

JsonArray& JsonArray::operator=(const JsonArray &arOther)
{
    JLOG("JsonArray copy assignment");
    JsonValue::operator=(static_cast<const JsonValue&>(arOther));
    mData.clear();
    for(auto el : arOther.mData) {
        mData.push_back(el->clone());
    }
    return *this;
}

JsonValue* JsonArray::clone() const
{
    JLOG("JsonArray::clone");
    auto result = new JsonArray();
    *result = *this;
    return result;
}

JsonArray& JsonArray::operator=(JsonArray &&arOther)
{
    JLOG("JsonArray move assignment");
    JsonValue::operator=(static_cast<JsonValue&>(arOther));
    mData = std::move(arOther.mData);
    arOther.mData.clear();
    return *this;
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

void JsonArray::toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2)
{
    std::string in(static_cast<std::string::size_type>(arPf.indent) * (aLevel + 1), ' ');
    std::string c = ",";

    arResult << "[" << arPf.nl;

    int rest = mData.size();
    for (auto el : mData) {
       arResult << in;
       el->toStringStream(arResult, arPf, aLevel+1, aForceToUCS2);
       if (--rest == 0) {
           c = "";
       }
       arResult << c << arPf.nl;
    }
    std::string in1(static_cast<std::string::size_type>(arPf.indent) * aLevel, ' ');
    arResult << in1 << "]";
}

} // rsp::utils::json

