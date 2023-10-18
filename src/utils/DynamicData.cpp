/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include <logging/Logger.h>
#include <utils/DynamicData.h>
#include <json/JsonEncoder.h>

using namespace rsp::logging;

//#define DDLOG(a) DLOG(a);
#define DDLOG(a)

namespace rsp::utils {

std::ostream& operator<< (std::ostream& os, const DynamicData& arValue)
{
    rsp::json::JsonEncoder je(true);
    os << je.Encode(arValue);
    return os;
}

/*
DynamicData::DynamicData(const DynamicData& arOther)
    : Variant(arOther),
      mName(arOther.mName),
      mItems(arOther.mItems)
{
    DDLOG("DynamicData copy constructor")
}

DynamicData::DynamicData(DynamicData&& arOther) noexcept
    : mName(std::move(arOther.mName)),
      mItems(std::move(arOther.mItems))
{
    DDLOG("DynamicData move constructor")
    mType = arOther.mType;
    mInt = arOther.mInt;
    mString = std::move(arOther.mString);
    arOther.mType = Types::Null;
}
*/

/*
DynamicData& DynamicData::operator =(const DynamicData& arOther)
{
    if (&arOther != this) {
        Variant::operator=(arOther);
        mName = arOther.mName;
        mItems = arOther.mItems;
        DDLOG("DynamicData copy assignment")
    }
    return *this;}

DynamicData& DynamicData::operator=(DynamicData&& arOther) noexcept
{
    if (&arOther != this) {
        mName = std::move(arOther.mName);
        mItems = std::move(arOther.mItems);
        Variant::operator=(std::move(arOther));
        DDLOG("DynamicData move assignment")
    }
    return *this;
}
*/

DynamicData& DynamicData::operator [](std::string_view aKey)
{
    DDLOG("DynamicData - Access member " << aKey)
    forceObject();
    for (DynamicData &v : mItems) {
        if (v.mName == aKey) {
            return v;
        }
    }
    THROW_WITH_BACKTRACE1(EMemberNotExisting, std::string(aKey));
}

const DynamicData& DynamicData::operator [](std::string_view aKey) const
{
    DDLOG("DynamicData - Getting member " << aKey)
    tryObject();
    for (const DynamicData &v : mItems) {
        if (v.mName == aKey) {
            return v;
        }
    }
    THROW_WITH_BACKTRACE1(EMemberNotExisting, std::string(aKey));
}

DynamicData& DynamicData::operator [](size_type aIndex)
{
    DDLOG("DynamicData - Access item " << aIndex)
    forceArray();
    return mItems.at(aIndex);
}

const DynamicData& DynamicData::operator [](size_type aIndex) const
{
    DDLOG("DynamicData - Getting item " << aIndex)
    tryArray();
    return mItems.at(aIndex);
}

DynamicData::size_type DynamicData::GetCount() const
{
    switch (mType) {
        default:
            return 0;

        case Types::Object:
        case Types::Array:
            return mItems.size();
    }
}

std::vector<std::string> DynamicData::GetMemberNames() const
{
    tryObject();
    std::vector<std::string> result;
    for (const DynamicData &jv : mItems) {
        result.push_back(jv.mName);
    }
    return result;
}

bool DynamicData::MemberExists(std::string_view aKey) const
{
    tryObject();
    for (const DynamicData &v : mItems) {
        if (v.mName == aKey) {
            return true;
        }
    }
    return false;
}

DynamicData& DynamicData::Add(DynamicData aValue)
{
    forceArray();
    Logger::GetDefault().Info() << "DynamicData::Add(): " << aValue;
    mItems.emplace_back(aValue);
    return *this;
}

DynamicData& DynamicData::Add(std::string_view aKey, DynamicData aValue)
{
    forceObject();
    Logger::GetDefault().Info() << "JsonObject::Add(): \"" << aKey << "\": " << aValue;
    aValue.mName = aKey;
    mItems.push_back(aValue);
    return *this;
}

DynamicData& DynamicData::Remove(size_type aIndex)
{
    tryArray();
    mItems.erase(mItems.begin() + aIndex);
    return *this;
}

DynamicData& DynamicData::Remove(std::string_view aKey)
{
    tryObject();
    auto it = std::find_if(mItems.begin(), mItems.end(), [&aKey](const DynamicData& arValue) {
        if (arValue.mName == aKey) {
            return true;
        }
        return false;
    });
    mItems.erase(it);
    return *this;
}

void DynamicData::Clear()
{
    mName.clear();
    mItems.clear();
    mType = Types::Null;
}

bool DynamicData::operator ==(const DynamicData &arOther) const
{
    if (this == &arOther) {
        return true;
    }
    if (mType != arOther.mType) {
        return false;
    }
    switch(mType) {
        case Types::Null:
            return true; // Types are the same, so both are null.

        case Types::Bool:
            return mBool == arOther.mBool;

        case Types::Int:
        case Types::Int64:
        case Types::Uint64:
        case Types::Uint32:
        case Types::Uint16:
            return mInt == arOther.mInt;

        case Types::Float:
            return mFloat == arOther.mFloat;

        case Types::Double:
            return mDouble == arOther.mDouble;

        case Types::Pointer:
            return mPointer == arOther.mPointer;

        case Types::String:
            return mString == arOther.mString;

        case Types::Array:
        case Types::Object: {
            if (GetCount() != arOther.GetCount()) {
                return false;
            }
            int i = 0;
            for (const DynamicData &jv : mItems) {
                const DynamicData &ref = (mType == Types::Object) ? arOther[jv.mName] : arOther[i++];
                if (!(jv == ref)) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

void DynamicData::tryArray() const
{
    if (!IsArray()) {
        THROW_WITH_BACKTRACE1(EDynamicTypeError, "DynamicData of type " + TypeToText() + " cannot be converted to Array");
    }
}

void DynamicData::tryObject() const
{
    if (!IsObject()) {
        THROW_WITH_BACKTRACE1(EDynamicTypeError, "DynamicData of type " + TypeToText() + " cannot be converted to Object");
    }
}

void DynamicData::forceArray()
{
    if (mType == Types::Null) {
        mType = Types::Array;
        return;
    }
    tryArray();
}

void DynamicData::forceObject()
{
    if (mType == Types::Null) {
        mType = Types::Object;
        return;
    }
    tryObject();
}

void DynamicData::Serializable::FromData(const DynamicData &arData)
{
    // Subclasses must implement this
    THROW_WITH_BACKTRACE(exceptions::NotImplementedException);
}

} /* namespace rsp::utils */
