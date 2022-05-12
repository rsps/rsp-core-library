/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef INCLUDE_UTILS_ENUMFLAGS_H_
#define INCLUDE_UTILS_ENUMFLAGS_H_

#include <cstdint>
#include <type_traits>

namespace rsp::utils {


template<typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
class EnumFlags
{
    T mValue;
public:
    using utype = typename std::underlying_type<T>::type;

    constexpr EnumFlags() : mValue(static_cast<T>(0)) {}

    template <typename N>
    constexpr EnumFlags(N aValue) : mValue(static_cast<T>(aValue)) {}


    constexpr operator T() const
    {
        return mValue;
    }

    constexpr operator utype() const
    {
        return static_cast<utype>(mValue);
    }

    constexpr explicit operator bool() const
    {
        return static_cast<utype>(mValue) != 0;
    }

    constexpr EnumFlags<T> operator&(T aValue)
    {
        return static_cast<T>(
            static_cast<utype>(mValue) &
            static_cast<utype>(aValue));
    }

    constexpr EnumFlags<T>& operator&=(T aValue)
    {
        mValue = static_cast<T>(
            static_cast<utype>(mValue) &
            static_cast<utype>(aValue));
        return *this;
    }

    constexpr EnumFlags<T> operator|(T aValue)
    {
        return static_cast<T>(
            static_cast<utype>(mValue) |
            static_cast<utype>(aValue));
    }

    constexpr bool operator==(T aValue)
    {
        return static_cast<utype>(mValue) == static_cast<utype>(aValue);
    }

    constexpr bool operator!=(T aValue)
    {
        return static_cast<utype>(mValue) != static_cast<utype>(aValue);
    }

    constexpr EnumFlags<T>& operator|=(T aValue)
    {
        mValue = static_cast<T>(
            static_cast<utype>(mValue) |
            static_cast<utype>(aValue));
        return *this;
    }

    constexpr EnumFlags<T> operator~()
    {
        return static_cast<T>(
            ~static_cast<utype>(mValue));
    }

} __attribute__((packed));

//template <class T>
//inline bool operator==(EnumFlags<T> a, EnumFlags<T> b) {
//    return a.mValue == b.mValue;
//}
//
//template <class T>
//inline bool operator!=(EnumFlags<T> a, EnumFlags<T> b) {
//    return !(a == b);
//}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator&(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<typename std::underlying_type<T>::type>(lhs) &
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator|(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<typename std::underlying_type<T>::type>(lhs) |
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator~(T rhs)
{
    return static_cast<T>(
        ~static_cast<typename std::underlying_type<T>::type>(rhs));
}

} // namespace rsp::utils

#endif /* INCLUDE_UTILS_ENUMFLAGS_H_ */
