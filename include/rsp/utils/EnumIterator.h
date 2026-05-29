/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef RSP_CORE_LIB_UTILS_ENUM_ITERATOR_H
#define RSP_CORE_LIB_UTILS_ENUM_ITERATOR_H

#include <iostream>
#include <ranges>
#include <type_traits>

namespace rsp::utils {

/**
 * \class EnumIterator
 * \brief Template helper class to create an iterator over continuous enum values
 *
 * Usage:
 * enum class MyEnum {
 *     begin,
 *     first = begin,
 *     second,
 *     third,
 *     end // not included
 * }
 *
 * class MyEnumRange
 * {
 * public:
 *     EnumIterator<MyEnum> begin() { return EnumIterator<MyEnum>(MyEnum::begin); }
 *     EnumIterator<MyEnum> end()   { return EnumIterator<MyEnum>(MyEnum::end); }
 * };
 *
 * \tparam T
 */
template <typename T>
class EnumIterator
{
public:
    static_assert(std::is_enum<T>::value, "EnumIterator must only be used with continuous enums!");

    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    EnumIterator(value_type aValue) : mValue(aValue) {}
    EnumIterator(pointer apValue) : mValue(*apValue) {}

    reference operator*() { return mValue; }
    pointer operator->() { return &mValue; }

    // Prefix increment
    EnumIterator<T>& operator++() { mValue = static_cast<T>(static_cast<int>(mValue)+1); return *this; }

    // Postfix increment
    EnumIterator<T> operator++(int) { EnumIterator<T> tmp = *this; ++(*this); return tmp; }

    friend bool operator== (const EnumIterator<T>& a, const EnumIterator<T>& b) { return a.mValue == b.mValue; };
    friend bool operator!= (const EnumIterator<T>& a, const EnumIterator<T>& b) { return a.mValue != b.mValue; };
private:
    T mValue;
};

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_ENUM_ITERATOR_H
