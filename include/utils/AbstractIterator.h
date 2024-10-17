/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_UTILS_ABSTRACT_ITERATOR_H
#define RSP_CORE_LIB_INCLUDE_UTILS_ABSTRACT_ITERATOR_H

#include <cstdint>
#include <iterator>

namespace rsp::utils {

/**
 * \brief Template class to define a forward iterator that can be used to traverse abstract types.
 * \tparam T Abstract type to iterate over
 *
 * For a full contiguous iterator \see https://stackoverflow.com/a/75061822
 */
template<class T>
struct AbstractIterator
{
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = value_type *;
    using reference         = value_type &;
    using size_type         = size_t;

    constexpr auto cAdd(pointer aPtr, difference_type aOffset) {
        if constexpr (std::is_const_v<value_type>) {
            return reinterpret_cast<pointer>(reinterpret_cast<const uint8_t *>(aPtr) + (aOffset * difference_type(mElementSize)));
        }
        else {
            return reinterpret_cast<pointer>(reinterpret_cast<uint8_t *>(aPtr) + (aOffset * difference_type(mElementSize)));
        }
    }

private:
    pointer mPtr = nullptr, mStart = nullptr, mSentinel = nullptr;
    size_type mElementSize = 0;
    static_assert(std::sentinel_for<decltype(mSentinel), decltype(mPtr)>);

public:
    AbstractIterator() { throw std::runtime_error("Not implemented"); }
    AbstractIterator(pointer aStart, pointer aSentinel, size_type aDerivedElementSize)
        : AbstractIterator(aStart, aStart, aSentinel, aDerivedElementSize) {}
    AbstractIterator(pointer aPtr, pointer aStart, pointer aSentinel, size_type aDerivedElementSize)
        : mPtr(aPtr), mStart(aStart), mSentinel(aSentinel), mElementSize(aDerivedElementSize) {}

    // Construct begin from vector
    template <class C>
    AbstractIterator(C &arContainer, bool aBegin)
        : AbstractIterator{ (aBegin ? &arContainer.front() : &arContainer.back() + 1), &arContainer.front(), &arContainer.back() + 1, sizeof(typename C::value_type) }
    {
    }

    reference operator*() const { return *mPtr; }
    pointer operator->() { return mPtr; }

    AbstractIterator operator+(const difference_type offset) const { return { cAdd(mPtr, offset), mStart, mSentinel, mElementSize }; }
    friend AbstractIterator operator+(const difference_type offset, const AbstractIterator &other) {
        return other + offset;
    }

    AbstractIterator& operator+=(const difference_type offset) { mPtr = cAdd(mPtr, offset); return *this; }

    // Prefix increment
    AbstractIterator& operator++() { mPtr = cAdd(mPtr, 1);; return *this; }

    // Postfix increment
    AbstractIterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }

    friend bool operator==(const AbstractIterator& a, const AbstractIterator& b) { return a.mPtr == b.mPtr; }
    friend bool operator!=(const AbstractIterator& a, const AbstractIterator& b) { return a.mPtr != b.mPtr; }
    auto operator<=>(const AbstractIterator &) const = default; // three-way comparison C++20

    auto begin() const { return AbstractIterator(mStart, mSentinel, mElementSize); }
    auto end() const { return AbstractIterator(mSentinel, mStart, mSentinel, mElementSize); }
};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_ABSTRACT_ITERATOR_H
