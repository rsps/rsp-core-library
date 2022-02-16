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

using namespace rsp::utils::json;

JsonArray::JsonArray()
{
    mType = Types::Pointer;
    mPointer = static_cast<uintptr_t>(JsonTypes::Array);
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




