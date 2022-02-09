/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_UTILS_VARIANT_H_
#define RSP_UTILS_VARIANT_H_

#include <utils/Nullable.h>
#include <string>
#include "CoreException.h"

namespace rsp::utils {

class EVariantException : public CoreException {
public:
    explicit EVariantException(const char *apMsg) : CoreException(apMsg) {}
};

class EConversionError : public EVariantException {
public:
    explicit EConversionError(const std::string aFrom, const std::string aTo) : EVariantException(std::string("Variant Conversion Error. From " + aFrom + " to " + aTo).c_str()) {}
};


class Variant : public Nullable
{
public:
    enum class Types {Null, Bool, Int, Int64, Uint64, Uint32, Float, Double, Pointer, String};

    Variant();
    Variant(bool aValue);
    Variant(int aValue);
    Variant(std::int64_t aValue);
    Variant(std::uint64_t aValue);
    Variant(std::uint32_t aValue);
    Variant(float aValue);
    Variant(double aValue);
    Variant(void* apValue);
    Variant(const std::string &arValue);
    Variant(const char *apValue);
    virtual ~Variant();

//    Variant(const Variant&) = delete;
//    Variant& operator=(const Variant&) = delete;

    bool IsNull() const override { return mType == Types::Null; }
    void Clear() override { mType = Types::Null; }
    Types GetType() const { return mType; }

    operator bool() const              { return AsBool(); }
    operator int() const               { return static_cast<int>(AsInt()); }
    operator std::int64_t() const      { return AsInt(); }
    operator std::uint64_t() const     { return static_cast<std::uint64_t>(AsInt()); }
    operator std::uint32_t() const     { return static_cast<std::uint32_t>(AsInt()); }
    operator float() const             { return static_cast<float>(AsDouble()); }
    operator double() const            { return AsDouble(); }
    operator void*() const             { return AsPointer(); }
    operator const std::string() const { return AsString(); }
    operator const char*() const       { return AsString().c_str(); }

    Variant& operator =(bool aValue);
    Variant& operator =(int aValue);
    Variant& operator =(std::int64_t aValue);
    Variant& operator =(std::uint64_t aValue);
    Variant& operator =(std::uint32_t aValue);
    Variant& operator =(float aValue);
    Variant& operator =(double aValue);
    Variant& operator =(void* apValue);
    Variant& operator =(const std::string &arValue);
    Variant& operator =(const char* apValue);

    bool AsBool() const;
    std::int64_t AsInt() const;
    double AsDouble() const;
    std::string AsString() const;
    void* AsPointer() const;

protected:
    Types mType;
    union {
        bool mBool;
        std::int64_t mInt;
        double mDouble;
        uintptr_t mPointer;
    };
    std::string mString{};

    std::string typeToText() const;
};

} /* namespace rsp::utils */

#endif /* RSP_UTILS_VARIANT_H_ */
