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
#include <utils/StructElement.h>

namespace rsp::utils {

/**
 * \class EVariantException
 * \brief Base exception for Variant class.
 *
 */
class EVariantException : public CoreException {
public:
    explicit EVariantException(const char *apMsg) : CoreException(apMsg) {}
};

/**
 * \class EConversionError
 * \brief Thrown if tried to read a value as a type which is not convertible from the vurrent type of a Variant.
 *
 */
class EConversionError : public EVariantException {
public:
    explicit EConversionError(const std::string aFrom, const std::string aTo) : EVariantException(std::string("Variant Conversion Error. From " + aFrom + " to " + aTo).c_str()) {}
};

/**
 * \class Variant
 * \brief Generic type class. Can hold all native types in the same object and often convert between them.
 *
 */
class Variant : public Nullable
{
public:
    /**
     * \enum Types
     * \brief Type declaration used for each native type.
     */
    enum class Types {Null, Bool, Int, Int64, Uint64, Uint32, Uint16, Float, Double, Pointer, String};

    /**
     * \fn  Variant()
     * \brief Construct an empty Variant (null)
     */
    Variant();

    Variant(const Variant &arOther);
    Variant(Variant &&arOther);

    template<class T>
    Variant(const rsp::utils::StructElement<T>& arOther) : Variant(ToVariant(arOther)) {}

    Variant& operator=(const Variant &arOther);
    Variant& operator=(Variant &&arOther);

    template<class T>
    Variant& operator=(const rsp::utils::StructElement<T>& arOther) {
        *this = ToVariant(arOther);
        return *this;
    }

    /**
     * \brief operator overload for Variant
     */
    template<class T>
    Variant ToVariant(const rsp::utils::StructElement<T>& arOther)
    {
        if (arOther.IsNull()) {
            return Variant();
        }
        return Variant(arOther.Get());
    }

    /**
     * \fn  Variant(...)
     * \brief Constructs a Variant with the given value and type of that value.
     *
     * \param aValue
     */
    Variant(bool aValue);
    Variant(int aValue);
    Variant(std::int64_t aValue);
    Variant(std::uint64_t aValue);
    Variant(std::uint32_t aValue);
    Variant(std::uint16_t aValue);
    Variant(float aValue);
    Variant(double aValue);
    Variant(void* apValue);
    Variant(const std::string &arValue);
    Variant(const char *apValue);

    /**
     * \fn  ~Variant()
     * \brief Virtual destructor
     */
    virtual ~Variant();

    /**
     * \fn bool IsNull()const
     * \brief Returns true if the content is null, not set or empty.
     *
     * \return bool
     */
    bool IsNull() const override { return mType == Types::Null; }
    /**
     * \fn void Clear()
     * \brief Resets the Variant content to null
     */
    void Clear() override { mType = Types::Null; }
    /**
     * \fn Types GetType()const
     * \brief Returns the current type of the Variant content.
     *
     * \return Variant::Types
     */
    Types GetType() const { return mType; }

    /**
     * \fn  operator <T>()const
     * \brief Operator overloads for all native types.
     */
    operator bool() const              { return AsBool(); }
    operator int() const               { return static_cast<int>(AsInt()); }
    operator std::int64_t() const      { return AsInt(); }
    operator std::uint64_t() const     { return static_cast<std::uint64_t>(AsInt()); }
    operator std::uint32_t() const     { return static_cast<std::uint32_t>(AsInt()); }
    operator std::uint16_t() const     { return static_cast<std::uint16_t>(AsInt()); }
    operator float() const             { return static_cast<float>(AsDouble()); }
    operator double() const            { return AsDouble(); }
    operator void*() const             { return AsPointer(); }
    operator const std::string() const { return AsString(); }
    operator const char*() const       { return AsString().c_str(); }

    /**
     * \fn Variant operator =&(T)
     * \brief Assignment operator overloads for all native types.
     *
     * \param aValue
     * \return Reference to this
     */
    Variant& operator =(bool aValue);
    Variant& operator =(int aValue);
    Variant& operator =(std::int64_t aValue);
    Variant& operator =(std::uint64_t aValue);
    Variant& operator =(std::uint32_t aValue);
    Variant& operator =(std::uint16_t aValue);
    Variant& operator =(float aValue);
    Variant& operator =(double aValue);
    Variant& operator =(void* apValue);
    Variant& operator =(const std::string &arValue);
    Variant& operator =(const char* apValue);

    /**
     * \fn bool AsBool()const
     * \brief Named conversion functions. Tries to convert the current value and type to the basic native return type.
     *
     * \return T
     */
    bool AsBool() const;
    std::int64_t AsInt() const;
    double AsDouble() const;
    float AsFloat() const { return static_cast<float>(AsDouble()); }
    std::string AsString() const;
    void* AsPointer() const;

protected:
    Types mType;
    union {
        bool mBool;
        std::int64_t mInt{0};
        float mFloat;
        double mDouble;
        uintptr_t mPointer;
    };
    std::string mString{};

    friend std::ostream& operator<< (std::ostream& os, Variant aValue);
    std::string typeToText() const;
};

std::ostream& operator<< (std::ostream& os, Variant aValue);

} /* namespace rsp::utils */

#endif /* RSP_UTILS_VARIANT_H_ */
