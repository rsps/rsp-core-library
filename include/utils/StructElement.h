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
 * Helper template class to make struct elements Nullable.
 *
 * The class also supports comparison with margins.
 */
template <class T>
class StructElement : public Nullable
{
public:
    StructElement() : mIsNull(true), mData(defaultItem<T>::default_value()), mMargin(defaultItem<T>::default_value())  {}
    StructElement(const T& aValue) : mIsNull(false), mData(aValue), mMargin(defaultItem<T>::default_value()) {}

    bool IsNull() const override { return mIsNull; }
    void Clear() override        { mIsNull = true; mData = static_cast<T>(0); }

    T Get() const {
        if (mIsNull) {
            THROW_WITH_BACKTRACE(ENullValueError);
        }
        return mData;
    }
    T Get(const T &arDefault) const {
        if (mIsNull) {
            return arDefault;
        }
        return mData;
    }
    operator T() const { return Get(); }

    void Set(T aValue) { mData = aValue; mIsNull = false; }
    StructElement<T>& operator =(const T& aValue) { Set(aValue); return *this; }

    bool Compare(const T& aValue) const {
        return (mIsNull) ? false : !differs(mData, aValue, mMargin);
    }

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

template <class T>
bool operator==(const StructElement<T>& aEl1, const StructElement<T>& aEl2 ) {
    return !(aEl1 != aEl2);
}

template <class T, class E>
bool operator!=(const StructElement<T>& aEl1, const StructElement<E>& aEl2 ) {
    THROW_WITH_BACKTRACE(ETypeMismatchError);
}

template <class T, class E>
bool operator==(const StructElement<T>& aEl1, const StructElement<E>& aEl2 ) {
    THROW_WITH_BACKTRACE(ETypeMismatchError);
}


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
