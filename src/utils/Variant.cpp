/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <sstream>
#include <cstdio>
#include <utils/Variant.h>
#include <logging/Logger.h>

//#define JLOG(a) DLOG(a)
#define JLOG(a)

namespace rsp::utils {

Variant::Variant()
    : mType(Types::Null),
      mPointer(reinterpret_cast<uintptr_t>(nullptr))
{
    JLOG("Variant default constructor");
}

Variant::Variant(const Variant &arOther)
    : mType(arOther.mType),
      mInt(arOther.mInt),
      mString(arOther.mString)
{
    JLOG("Variant copy constructor");
}

Variant::Variant(Variant &&arOther)
    : mType(arOther.mType),
      mInt(arOther.mInt),
      mString(arOther.mString)
{
    JLOG("Variant move constructor");
    arOther.mType = Types::Null;
}

Variant& Variant::operator=(const Variant &arOther)
{
    if (&arOther != this) {
        JLOG("Variant copy assignment");
        mType = arOther.mType;
        mInt = arOther.mInt,
        mString = arOther.mString;
    }
    return *this;
}

Variant& Variant::operator=(Variant &&arOther)
{
    if (&arOther != this) {
        JLOG("Variant move assignment");
        mType = arOther.mType;
        mInt = std::move(arOther.mInt),
        mString = std::move(arOther.mString);
        arOther.mType = Types::Null;
    }
    return *this;
}


Variant::Variant(bool aValue)
    : mType(Types::Bool),
      mBool(aValue)
{
    JLOG("Variant bool constructor");
}

Variant::Variant(int aValue)
    : mType(Types::Int),
      mInt(aValue)
{
    JLOG("Variant int constructor");
}

Variant::Variant(std::int64_t aValue)
    : mType(Types::Int64),
      mInt(aValue)
{
    JLOG("Variant int64 constructor");
}

Variant::Variant(std::uint64_t aValue)
    : mType(Types::Uint64),
      mInt(static_cast<std::int64_t>(aValue))
{
    JLOG("Variant uint64 constructor");
}

Variant::Variant(std::uint32_t aValue)
    : mType(Types::Uint32),
      mInt(aValue)
{
    JLOG("Variant uint32 constructor");
}

Variant::Variant(std::uint16_t aValue)
    : mType(Types::Uint16),
      mInt(aValue)
{
    JLOG("Variant uint16 constructor");
}

Variant::Variant(float aValue)
    : mType(Types::Float),
      mFloat(aValue)
{
    JLOG("Variant float constructor");
}

Variant::Variant(double aValue)
    : mType(Types::Double),
      mDouble(aValue)
{
    JLOG("Variant double constructor");
}

Variant::Variant(void *apValue)
    : mType(Types::Pointer),
      mPointer(reinterpret_cast<uintptr_t>(apValue))
{
    JLOG("Variant pointer constructor");
}

Variant::Variant(const std::string &arValue)
    : mType(Types::String),
      mString(arValue)
{
}

Variant::Variant(const char *apValue)
    : mType(Types::String),
      mString(apValue)
{
}

Variant::~Variant()
{
}

Variant& Variant::operator =(bool aValue)
{
    mType = Types::Bool;
    mBool = aValue;
    return *this;
}

Variant& Variant::operator =(int aValue)
{
    mType = Types::Int;
    mInt = aValue;
    return *this;
}

Variant& Variant::operator =(std::int64_t aValue)
{
    mType = Types::Int64;
    mInt = aValue;
    return *this;
}

Variant& Variant::operator =(std::uint64_t aValue)
{
    mType = Types::Uint64;
    mInt = static_cast<std::int64_t>(aValue);
    return *this;
}

Variant& Variant::operator =(std::uint32_t aValue)
{
    mType = Types::Uint32;
    mInt = aValue;
    return *this;
}

Variant& Variant::operator =(std::uint16_t aValue)
{
    mType = Types::Uint16;
    mInt = aValue;
    return *this;
}

Variant& Variant::operator =(float aValue)
{
    mType = Types::Float;
    mFloat = aValue;
    return *this;
}

Variant& Variant::operator =(double aValue)
{
    mType = Types::Double;
    mDouble = aValue;
    return *this;
}

Variant& Variant::operator =(void *apValue)
{
    mType = Types::Pointer;
    mPointer = reinterpret_cast<uintptr_t>(apValue);
    return *this;
}

Variant& Variant::operator =(const std::string &arValue)
{
    mType = Types::String;
    mString = arValue;
    return *this;
}

Variant& Variant::operator =(const char *apValue)
{
    mType = Types::String;
    mString = apValue;
    return *this;
}

bool Variant::AsBool() const
{
    switch (mType) {
        case Types::Null:
            return false;

        case Types::Bool:
            return mBool;

        case Types::Int:
        case Types::Int64:
        case Types::Uint64:
        case Types::Uint32:
        case Types::Uint16:
            return (mInt != 0);

        case Types::Float:
            return (std::fabs(mFloat) < 0.001f);

        case Types::Double:
            return (std::fabs(mDouble) < 0.0001f);

        case Types::Pointer:
            return (mPointer != reinterpret_cast<uintptr_t>(nullptr));

        case Types::String:
            if (mString == "true" || mString == "1") {
                return true;
            }
            else if (mString == "false" || mString == "0" || mString == "null") {
                return false;
            }
            return (mString.length() > 0);

        default:
            THROW_WITH_BACKTRACE2(EConversionError, typeToText(), "bool");
            break;
    }
    return false;
}

std::int64_t Variant::AsInt() const
{
    switch (mType) {
        case Types::Bool:
            return static_cast<std::int64_t>(mBool);

        case Types::Int:
        case Types::Int64:
        case Types::Uint64:
        case Types::Uint32:
        case Types::Uint16:
            return mInt;

        case Types::Float:
            return static_cast<std::int64_t>(mFloat);
        case Types::Double:
            return static_cast<std::int64_t>(mDouble);

        case Types::Pointer:
            return static_cast<std::int64_t>(mPointer);

        case Types::String:
            return static_cast<std::int64_t>(std::strtol(mString.c_str(), nullptr, 0));

        default:
            THROW_WITH_BACKTRACE2(EConversionError, typeToText(), "int");
            break;
    }
    return 0;
}

double Variant::AsDouble() const
{
    switch (mType) {
        case Types::Bool:
            return static_cast<double>(static_cast<int>(mBool));

        case Types::Int:
        case Types::Int64:
        case Types::Uint64:
        case Types::Uint32:
        case Types::Uint16:
            return static_cast<double>(mInt);

        case Types::Float:
            return static_cast<double>(mFloat);
        case Types::Double:
            return mDouble;

        case Types::Pointer:
            return static_cast<double>(mPointer);

        case Types::String:
            return std::strtod(mString.c_str(), nullptr);

        default:
            THROW_WITH_BACKTRACE2(EConversionError, typeToText(), "double");
            break;
    }
    return 0.0f;
}


template <typename T>
std::string to_string_with_high_precision(const T a_value)
{
    const unsigned int digits = std::numeric_limits<T>::max_digits10;
    std::ostringstream out;
//    out << "digits " << digits << " ";
    out.precision(digits);
//    out << std::fixed;
    out << a_value;
    return out.str();
}

std::string Variant::AsString() const
{
    switch (mType) {
        case Types::Null:
            return "null";

        case Types::Bool:
            return mBool ? "true" : "false";

        case Types::Int:
        case Types::Int64:
            return std::to_string(mInt);

        case Types::Uint64:
        case Types::Uint32:
        case Types::Uint16:
            return std::to_string(static_cast<uint64_t>(mInt));

        case Types::Float:
            return to_string_with_high_precision(mFloat);
        case Types::Double:
            return to_string_with_high_precision(mDouble);

        case Types::Pointer:
        {
            char buf[40];
            sprintf(buf, "%p", reinterpret_cast<void*>(mPointer));
            return std::string(buf);
        }

        case Types::String:
            return mString;

        default:
            THROW_WITH_BACKTRACE2(EConversionError, typeToText(), "string");
            break;
    }
    return "";
}

void* Variant::AsPointer() const
{
    switch (mType) {
        case Types::Null:
            return nullptr;

        case Types::Pointer:
            return reinterpret_cast<void*>(mPointer);

        default:
            THROW_WITH_BACKTRACE2(EConversionError, typeToText(), "pointer");
            break;
    }
    return nullptr;
}

std::string Variant::typeToText() const
{
    switch (mType) {
        default: return std::string("Unknown type (") + std::to_string(static_cast<int>(mType)) + ")";
        case Types::Null: return "null";
        case Types::Bool: return "bool";
        case Types::Int: return "int";
        case Types::Int64: return "int64";
        case Types::Uint64: return "uint64";
        case Types::Uint32: return "uint32";
        case Types::Uint16: return "uint16";
        case Types::Float: return "float";
        case Types::Double: return "double";
        case Types::Pointer: return "pointer";
        case Types::String: return "string";
    }
}

std::ostream& operator<< (std::ostream& os, Variant aValue)
{
    os << aValue.typeToText() << ":" << aValue.AsString();
    return os;
}

} /* namespace rsp::utils */
