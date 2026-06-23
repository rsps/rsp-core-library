/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SECURITY_SECURE_ALLOCATOR_H
#define RSP_CORE_LIB_SECURITY_SECURE_ALLOCATOR_H

#include <memory>
#include <limits>

#ifdef _WIN32
#include <windows.h> // for SecureZeroMemory()
#else
#include <string.h> // for explicit_bzero()
#endif

namespace rsp::security {

/**
 * \brief A template implementation of a secure allocator.
 *
 * Memory is always cleared when freed.
 *
 * \see https://howardhinnant.github.io/allocator_boilerplate.html
 *
 * \tparam T The element type to use by the allocator
 */
template <class T>
class SecureAllocator
{
public:
    using value_type = T;

    using pointer = value_type*;
    using const_pointer = typename std::pointer_traits<pointer>::template rebind<const value_type>;
    using void_pointer = typename std::pointer_traits<pointer>::template rebind<void>;
    using const_void_pointer = typename std::pointer_traits<pointer>::template rebind<const void>;
    using difference_type = typename std::pointer_traits<pointer>::difference_type;
    using size_type = std::make_unsigned_t<difference_type>;

    template <class U>
    struct rebind
    {
        typedef SecureAllocator<U> other;
    };

    SecureAllocator() noexcept = default; // not required, unless used

    template <class U>
    explicit SecureAllocator(const SecureAllocator<U>&) noexcept
    {
    }

    pointer allocate(std::size_t n)
    {
        return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
    }

    void deallocate(pointer p, std::size_t sz) noexcept
    {
        cleanse(p, sz);
        ::operator delete(p);
    }

    /**
     * \brief Helper method to clear a memory range
     * \param p
     * \param n
     */
    void cleanse(pointer p, std::size_t n)
    {
        // no memset() here because it might be optimized out by the compiler
#ifdef _WIN32
        SecureZeroMemory(p, n * sizeof(T));
#else
        explicit_bzero(p, n * sizeof(T));
#endif
    }

    [[nodiscard]] std::size_t max_size() const noexcept
    {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }
};

template <class T, class U>
bool operator==(const SecureAllocator<T>&, const SecureAllocator<U>&) noexcept
{
    return true;
}

template <class T, class U>
bool operator!=(const SecureAllocator<T>& x, const SecureAllocator<U>& y) noexcept
{
    return !(x == y);
}

} // namespace rsp::security

#endif // RSP_CORE_LIB_SECURITY_SECURE_ALLOCATOR_H
