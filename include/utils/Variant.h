/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_VARIANT_H
#define RSP_CORE_LIB_UTILS_VARIANT_H

#include <exceptions/CoreException.h>
#include <utils/Nullable.h>
#include <string>
#include <utils/StructElement.h>

namespace rsp::utils {

/**
 * \class EVariantException
 * \brief Base exception for Variant class.
 *
 */
class EVariantException : public exceptions::CoreException {
public:
    explicit EVariantException(const char *apMsg) : CoreException(apMsg) {}
    explicit EVariantException(const std::string& arMsg) : CoreException(arMsg) {}
};

/**
 * \class EConversionError
 * \brief Thrown if tried to read a value as a type which is not convertible from the current type of a Variant.
 */
class EConversionError : public EVariantException {
public:
    explicit EConversionError(const std::string& arFrom, const std::string& arTo) : EVariantException(std::string("Variant Conversion Error. From " + arFrom + " to " + arTo)) {}
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
    enum class Types : uint32_t {Null, Bool, Int, Int64, Uint64, Uint32, Uint16, Float, Double, Pointer, String, Object, Array};

    /**
     * \fn  Variant()
     * \brief Construct an empty Variant (null)
     */
    Variant();

    Variant(const Variant &arOther);
    Variant(Variant &&arOther) noexcept;

    template<class T>
    explicit Variant(const rsp::utils::StructElement<T>& arOther)
        : Variant(ToVariant(arOther))
    {
    }

    Variant& operator=(const Variant &arOther);
    Variant& operator=(Variant &&arOther) noexcept ;

    template<class T>
    Variant& operator=(const rsp::utils::StructElement<T>& arOther) {
        *this = ToVariant(arOther);
        return *this;
    }

    /**
     * \brief operator overload for Variant
     */
    template<class T>
    Variant ToVariant(const rsp::utils::StructElement<T>& arOther) noexcept
    {
        if (arOther.IsNull()) {
            return {};
        }
        return Variant(arOther.Get());
    }

    /**
     * \fn  Variant(...)
     * \brief Constructs a Variant with the given value and type of that value.
     *
     * \param aValue
     */
    // NOLINTBEGIN, Conversion constructors.
    Variant(bool aValue);
    Variant(int aValue);
    Variant(int64_t aValue);
    Variant(uint64_t aValue);
    Variant(uint32_t aValue);
    Variant(uint16_t aValue);
    Variant(float aValue);
    Variant(double aValue);
    Variant(void* apValue);
    Variant(const std::string& arValue);
    Variant(std::string &&arValue) noexcept;
    Variant(const char *apValue);
    // NOLINTEND

    /**
     * \fn bool IsNull()const
     * \brief Returns true if the content is null, not set or empty.
     *
     * \return bool
     */
    [[nodiscard]] bool IsNull() const override { return mType == Types::Null; }
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
    [[nodiscard]] Types GetType() const { return mType; }

    /**
     * \brief Get a textual representation of the Variant content type
     * \return string
     */
    [[nodiscard]] std::string TypeToText() const;

    /**
     * \fn  operator <T>()const
     * \brief Conversion operators for all native types.
     */
    // NOLINTBEGIN, Conversion operators.
    operator bool() const              { return AsBool(); }
    operator int() const               { return static_cast<int>(AsInt()); }
    operator int64_t() const      { return AsInt(); }
    operator uint64_t() const     { return static_cast<uint64_t>(AsInt()); }
    operator uint32_t() const     { return static_cast<uint32_t>(AsInt()); }
    operator uint16_t() const     { return static_cast<uint16_t>(AsInt()); }
    operator float() const             { return static_cast<float>(AsDouble()); }
    operator double() const            { return AsDouble(); }
    operator void*() const             { return AsPointer(); }
    operator std::string() const { return AsString(); }
    // NOLINTEND

    /**
     * \fn Variant operator =&(T)
     * \brief Assignment operator overloads for all native types.
     *
     * \param aValue
     * \return Reference to this
     */
    Variant& operator =(bool aValue);
    Variant& operator =(int aValue);
    Variant& operator =(int64_t aValue);
    Variant& operator =(uint64_t aValue);
    Variant& operator =(uint32_t aValue);
    Variant& operator =(uint16_t aValue);
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
    [[nodiscard]] bool AsBool() const;
    [[nodiscard]] int64_t AsInt() const;
    [[nodiscard]] double AsDouble() const;
    [[nodiscard]] float AsFloat() const { return static_cast<float>(AsDouble()); }
    [[nodiscard]] std::string AsString() const;
    [[nodiscard]] void* AsPointer() const;

    [[nodiscard]] int64_t RawAsInt() const { return mInt; }

protected:
    Types mType = Types::Null;
    union {
        bool mBool;
        int64_t mInt{0};
        float mFloat;
        double mDouble;
        uintptr_t mPointer;
    };
    std::string mString{};
};

std::ostream& operator<< (std::ostream& os, const Variant& arValue);

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_VARIANT_H
