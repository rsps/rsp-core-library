/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cmath>
#include <concepts>
#include "ConstTypeInfo.h"
#include "Nullable.h"
#include "BinaryStream.h"
#include <magic_enum.hpp>

#ifndef RSP_CORE_LIB_UTILS_STRUCT_ELEMENT_H
#define RSP_CORE_LIB_UTILS_STRUCT_ELEMENT_H

namespace rsp::utils {

/**
 * Template class to return a default value for the given type.
 *
 * \tparam T
 */
template<class T> class defaultItem {
public:
    static constexpr T default_value() { return T(0); }
};

/**
 * Specialized default value for string types.
 */
template<> class defaultItem<std::string> {

public:
    static constexpr const char *default_value() { return ""; }
};

/**
 * \class StructElement<T>
 * \brief Helper template class to make variables and struct elements Nullable.
 *
 * The class also supports comparison with margins (epsilon).
 */
template <class T>
class StructElementBase : public Nullable, public BinaryStreamable
{
public:
    /**
     * \fn  StructElement()
     * \brief Constructor of empty (Null) StructElement
     */
    StructElementBase() : mIsNull(true), mData(T(defaultItem<T>::default_value())), mMargin(T(defaultItem<T>::default_value()))  {}
    /**
     * \fn  StructElement(const T&)
     * \brief Constructor of StructElement with type T and value.
     *
     * \param aValue
     */
    StructElementBase(const T& aValue) : mIsNull(false), mData(aValue), mMargin(defaultItem<T>::default_value()) {} // NOLINT, Conversion constructor

    StructElementBase(const StructElementBase<T> &arOther) = default;
    StructElementBase(StructElementBase<T> &&arOther) = default;

    StructElementBase& operator=(const StructElementBase<T> &arOther) = default;
    StructElementBase& operator=(StructElementBase<T> &&arOther) = default;

    /**
     * \fn bool IsNull()const
     * \brief Returns true if content is null, not set or uninitialized.
     *
     * \return bool
     */
    [[nodiscard]] bool IsNull() const override { return mIsNull; }

    /**
     * \fn void Clear()
     * \brief Clears the content and set the type to null.
     */
    void Clear() override        { mIsNull = true; mData = {}; }

    /**
     * \brief Conversion operator overload.
     */
    operator T() const { return get(); } // NOLINT, Conversion operator

    /**
     * \fn void Set(T)
     * \brief Setter that changes the content and the type the type of the value.
     *
     * \param aValue
     */
    void Set(const T& aValue) { mData = aValue; mIsNull = false; }
    /**
     * \fn StructElement<T> operator =&(const T&)
     * \brief Assignment operator that changes the content and the type.
     *
     * \param aValue
     * \return Reference to this.
     */
    virtual StructElementBase<T>& operator=(const T& aValue) { Set(aValue); return *this; }

    /**
     * \fn bool Compare(const T&)const
     * \brief Compares the content with the given value, respecting the preset margin (epsilon).
     *
     * \param aValue
     * \return bool
     */
    [[nodiscard]] bool Compare(const T& aValue) const {
        return !(mIsNull) && !differs(mData, aValue, mMargin);
    }

    /**
     * \fn bool operator ==(const T&)const
     * \brief Equality operator overload that respect the preset margin.
     *
     * \param aValue
     * \return bool
     */
    bool operator==(const T& aValue) const {
        return Compare(aValue);
    }

    /**
     * \fn void SetMargin(T)
     * \brief Set the margin (epsilon) used during comparison operations.
     *
     * \param aValue
     */
    StructElementBase<T>& SetMargin(T aValue) { mMargin = aValue; return *this; }
    [[nodiscard]] T GetMargin() const { return mMargin; }

    BinaryStream& SaveTo(BinaryStream &o) const override
    {
        o << mIsNull;
        if (!mIsNull) {
            o << mData;
            if constexpr (std::is_arithmetic_v<T>) {
                o << mMargin;
            }
        }
        return o;
    }

    BinaryStream& LoadFrom(BinaryStream &i) override
    {
        i >> mIsNull;
        if (!mIsNull) {
            i >> mData;
            if constexpr (std::is_arithmetic_v<T>) {
                i >> mMargin;
            }
        }
        return i;
    }

protected:
    template <class E>
    friend std::ostream & operator<< (std::ostream &out, StructElementBase<E> const &t);

    template <class E>
    friend bool operator!=(const StructElementBase<E>& aEl1, const StructElementBase<E>& aEl2 );

    template <class E>
    friend bool operator==(const StructElementBase<E>& aEl1, const StructElementBase<E>& aEl2 );

    bool mIsNull;
    T mData;
    T mMargin;

    [[nodiscard]] T get() const
    {
        if (mIsNull) {
            THROW_WITH_BACKTRACE1(ENullValueError, NameOf<T>());
        }
        return mData;
    }

    [[nodiscard]] T get(const T &arDefault) const
    {
        if (mIsNull) {
            return arDefault;
        }
        return mData;
    }

    /*
     * Template function for difference check
     */
    [[nodiscard]] bool differs(T aVal1, T aVal2, T aMargin) const
    {
        if constexpr(std::is_floating_point<T>::value) {
            return std::fabs(aVal1 - aVal2) > aMargin;
        } else if constexpr(std::is_integral<T>::value) {
            return ((aVal1 > aVal2) ? aVal1 - aVal2 : aVal2 - aVal1) > aMargin;
        }
        else {
            return (aVal1 != aVal2);
        }
    }
};

template <class T>
class StructElement : public StructElementBase<T>
{
public:
    using StructElementBase<T>::StructElementBase;

    StructElement& operator=(const T& aValue) override { StructElementBase<T>::Set(aValue); return *this; }

    [[nodiscard]] T Get() const { return StructElementBase<T>::get(); }
    [[nodiscard]] T Get(const T &arDefault) const { return StructElementBase<T>::get(arDefault); }
};

template <class E> requires (std::is_enum_v<E>)
class StructElement<E> : public StructElementBase<typename std::underlying_type<E>::type>
{
public:
    using T = std::underlying_type<E>::type;
    using StructElementBase<T>::StructElementBase;

    explicit StructElement(E aValue) : StructElementBase<T>(T(aValue)) {}

    StructElement& operator=(const T& aValue) override { StructElementBase<T>::Set(aValue); return *this; }
    StructElement& operator=(E aValue) { StructElementBase<T>::Set(T(aValue)); return *this; }

    [[nodiscard]] E Get() const { return E(StructElementBase<T>::get()); }
    [[nodiscard]] E Get(const E&arDefault) const { return E(StructElementBase<T>::get(T(arDefault))); }
};

template <class T> requires std::is_floating_point_v<T>
class StructElement<T> : public StructElementBase<T>
{
public:
    using StructElementBase<T>::StructElementBase;

    StructElement(const T& aValue) : StructElementBase<T>(aValue) {} // NOLINT, Conversion constructor

    StructElement(const StructElement<T> &arOther) : StructElementBase<T>(arOther), mPrecision(arOther.mPrecision) {}
    StructElement(StructElement<T> &&arOther) noexcept : StructElementBase<T>(arOther), mPrecision(std::move(arOther.mPrecision)) {}
    StructElement& operator=(const StructElement<T> &arOther) {
        StructElementBase<T>::operator=(arOther);
        mPrecision = arOther.mPrecision;
        return *this;
    }
    StructElement& operator=(StructElement<T> &&arOther) noexcept {
        mPrecision = std::move(arOther.mPrecision);
        StructElementBase<T>::operator=(std::move(arOther));
        return *this;
    }
    StructElement& operator=(const T& aValue) override { StructElementBase<T>::Set(aValue); return *this; } // Without this, constructor + copy is called

    StructElement& SetPrecision(int aPrecision) { mPrecision = aPrecision; return *this; }
    [[nodiscard]] int GetPrecision() const { return mPrecision; }

    BinaryStream& SaveTo(BinaryStream &o) const override
    {
        StructElementBase<T>::SaveTo(o);
        if (!StructElementBase<T>::IsNull()) {
            o << mPrecision;
        }
        return o;
    }

    BinaryStream& LoadFrom(BinaryStream &i) override
    {
        StructElementBase<T>::LoadFrom(i);
        if (!StructElementBase<T>::IsNull()) {
            i >> mPrecision;
        }
        return i;
    }

    [[nodiscard]] T Get() const { return StructElementBase<T>::get(); }
    [[nodiscard]] T Get(const T &arDefault) const { return StructElementBase<T>::get(arDefault); }

protected:
    friend class Variant;
    int mPrecision = -1;
};

template <class T>
bool operator!=(const StructElementBase<T>& aEl1, const StructElementBase<T>& aEl2 ) {
    if (aEl1.mIsNull) {
        return !aEl2.mIsNull;
    }
    if (aEl2.mIsNull) {
        return true;
    }
    return aEl1.differs(aEl1.mData, aEl2.mData, aEl1.mMargin);

}

/**
 * \fn bool operator ==(const StructElement<T>&, const StructElement<T>&)
 * \brief Comparison operator for StructElements of same type.
 *
 * \tparam T
 * \param aEl1
 * \param aEl2
 * \return bool
 */
template <class T>
bool operator==(const StructElementBase<T>& aEl1, const StructElementBase<T>& aEl2 ) {
    return !(aEl1 != aEl2);
}

/**
 * \fn bool operator !=(const StructElement<T>&, const StructElement<E>&)
 * \brief In-equality operator for StructElements of different types. Always throws.
 *
 * \tparam T
 * \tparam E
 * \param aEl1
 * \param aEl2
 * \return
 */
template <class T, class E> requires (!std::is_same_v<T, E>)
bool operator!=(const StructElementBase<T>& /*aEl1*/, const StructElementBase<E>& /*aEl2*/ ) {
    static_assert(std::is_same_v<T, E>, "Value types must be same");
    return false;
}

/**
 * \fn bool operator !=(const StructElement<T>&, const StructElement<E>&)
 * \brief Equality operator for StructElements of different types. Always throws.
 *
 * \tparam T
 * \tparam E
 * \param aEl1
 * \param aEl2
 * \return
 */
template <class T, class E> requires (!std::is_same_v<T, E>)
bool operator==(const StructElement<T>& /*aEl1*/, const StructElement<E>& /*aEl2*/ ) {
    static_assert(std::is_same_v<T, E>, "Value types must be same");
    return false;
}


// Default enum streaming
template <class E> requires std::is_enum_v<E>
std::ostream & operator<< (std::ostream &o, E value) {
    o << magic_enum::enum_name(value);
    return o;
}

/**
 * \fn std::ostream operator <<&(std::ostream&, const StructElement<T>&)
 * \brief Streaming operator for debugging Variant content
 *
 * \tparam T
 * \param out
 * \param t
 * \return out
 */
template <class T>
std::ostream & operator<< (std::ostream &out, StructElement<T> const &t) {
    if (t.IsNull()) {
        out << "null";
    }
    else {
        out << t.Get();
    }
    return out;
}

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_STRUCT_ELEMENT_H
