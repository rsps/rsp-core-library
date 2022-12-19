/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "Nullable.h"

#ifndef INCLUDE_UTILS_STRUCTELEMENT_H_
#define INCLUDE_UTILS_STRUCTELEMENT_H_

namespace rsp::utils {

/**
 * Template class to return a default value for the given type.
 *
 * \tparam T
 */
template<class T> class defaultItem {
public:
    static constexpr T default_value() { return 0; }
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
class StructElement : public Nullable
{
public:
    /**
     * \fn  StructElement()
     * \brief Constructor of empty (Null) StructElement
     */
    StructElement() : mIsNull(true), mData(defaultItem<T>::default_value()), mMargin(defaultItem<T>::default_value())  {}
    /**
     * \fn  StructElement(const T&)
     * \brief Constructor of StructElement with type T and value.
     *
     * \param aValue
     */
    StructElement(const T& aValue) : mIsNull(false), mData(aValue), mMargin(defaultItem<T>::default_value()) {}

    StructElement(const StructElement<T> &arOther) = default;
    StructElement(StructElement<T> &&arOther) = default;

    StructElement& operator=(const StructElement<T> &arOther) = default;
    StructElement& operator=(StructElement<T> &&arOther) = default;

    /**
     * \fn bool IsNull()const
     * \brief Returns true if content is null, not set or uninitialized.
     *
     * \return bool
     */
    bool IsNull() const override { return mIsNull; }

    /**
     * \fn void Clear()
     * \brief Clears the content and set the type to null.
     */
    void Clear() override        { mIsNull = true; mData = static_cast<T>(0); }

    /**
     * \fn T Get()const
     * \brief Getter that throws if content is null.
     *
     * \return T
     */
    T Get() const {
        if (mIsNull) {
            THROW_WITH_BACKTRACE(ENullValueError);
        }
        return mData;
    }
    /**
     * \fn T Get(const T&)const
     * \brief Getter that returns the given default in case content is null.
     *
     * \param arDefault
     * \return T
     */
    T Get(const T &arDefault) const {
        if (mIsNull) {
            return arDefault;
        }
        return mData;
    }

    /**
     * \fn  operator #0()const
     * \brief Operator overload.
     */
    operator T() const { return Get(); }

    /**
     * \fn void Set(T)
     * \brief Setter that changes the content and the type the type of the value.
     *
     * \param aValue
     */
    void Set(T aValue) { mData = aValue; mIsNull = false; }
    /**
     * \fn StructElement<T> operator =&(const T&)
     * \brief Assignment operator that changes the content and the type.
     *
     * \param aValue
     * \return Reference to this.
     */
    StructElement<T>& operator =(const T& aValue) { Set(aValue); return *this; }

    /**
     * \fn bool Compare(const T&)const
     * \brief Compares the content with the given value, respecting the preset margin (epsilon).
     *
     * \param aValue
     * \return bool
     */
    bool Compare(const T& aValue) const {
        return (mIsNull) ? false : !differs(mData, aValue, mMargin);
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
    void SetMargin(T aValue) { mMargin = aValue; }

protected:
    template <class E>
    friend std::ostream & operator<< (std::ostream &out, StructElement<E> const &t);

    template <class E>
    friend bool operator!=(const StructElement<E>& aEl1, const StructElement<E>& aEl2 );

    template <class E>
    friend bool operator==(const StructElement<E>& aEl1, const StructElement<E>& aEl2 );

    bool mIsNull;
    T mData;
    T mMargin;

    /*
     * Simple overloaded functions for difference check
     */
    bool differs(double aVal1, double aVal2, double aMargin) const {
        return std::fabs(aVal1 - aVal2) > aMargin;
    }

    bool differs(float aVal1, float aVal2, float aMargin) const {
        return std::fabs(aVal1 - aVal2) > aMargin;
    }

    bool differs(int aVal1, int aVal2, int aMargin) const {
        return std::abs(aVal1 - aVal2) > aMargin;
    }

    bool differs(unsigned int aVal1, unsigned int aVal2, unsigned int aMargin) const {
        return std::abs(static_cast<int>(aVal1 - aVal2)) > aMargin;
    }

    bool differs(bool aVal1, bool aVal2, bool) const {
        return (aVal1 != aVal2);
    }
};


template <class T>
bool operator!=(const StructElement<T>& aEl1, const StructElement<T>& aEl2 ) {
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
bool operator==(const StructElement<T>& aEl1, const StructElement<T>& aEl2 ) {
    return !(aEl1 != aEl2);
}

/**
 * \fn bool operator !=(const StructElement<T>&, const StructElement<E>&)
 * \brief In-eaquality operator for StructElements of different types. Always throws.
 *
 * \tparam T
 * \tparam E
 * \param aEl1
 * \param aEl2
 * \return
 */
template <class T, class E>
bool operator!=(const StructElement<T>& aEl1, const StructElement<E>& aEl2 ) {
    THROW_WITH_BACKTRACE(ETypeMismatchError);
}

/**
 * \fn bool operator !=(const StructElement<T>&, const StructElement<E>&)
 * \brief Eaquality operator for StructElements of different types. Always throws.
 *
 * \tparam T
 * \tparam E
 * \param aEl1
 * \param aEl2
 * \return
 */
template <class T, class E>
bool operator==(const StructElement<T>& aEl1, const StructElement<E>& aEl2 ) {
    THROW_WITH_BACKTRACE(ETypeMismatchError);
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
    if (t.mIsNull) {
        out << "null";
    }
    else {
        out << t.mData;
    }
    return out;
}

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_STRUCTELEMENT_H_ */
