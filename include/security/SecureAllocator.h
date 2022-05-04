/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_SECUREALLOCATOR_H_
#define INCLUDE_SECURITY_SECUREALLOCATOR_H_

#include <memory>
#include <limits>

namespace rsp::security {


template <typename T>
struct SecureAllocator: public std::allocator<T>
{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    pointer address (reference v) const {return &v;}
    const_pointer address (const_reference v) const {return &v;}

    pointer allocate (size_type n, const void* hint = 0) {
        if (n > std::numeric_limits<size_type>::max() / sizeof(T))
            throw std::bad_alloc();
        return static_cast<pointer> (::operator new (n * sizeof (value_type)));
    }

    void deallocate(pointer p, size_type n) {
        std::fill_n(p, n*sizeof(T), -1);
        ::operator delete(p);
    }

    size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof (T);
    }

    template<typename U>
    struct rebind
    {
        typedef SecureAllocator<U> other;
    };

    void construct (pointer ptr, const T& val) noexcept {
        new (static_cast<T*>(ptr) ) T (val);
    }

    void destroy(pointer ptr) noexcept {
        static_cast<T*>(ptr)->~T();
    }

#if __cplusplus >= 201103L
    template<typename U, typename... Args>
    void construct (U* ptr, Args&&  ... args) noexcept {
        ::new (static_cast<void*> (ptr) ) U (std::forward<Args> (args)...);
    }

    template<typename U>
    void destroy(U* ptr) {
        ptr->~U();
    }
#endif
};


} // namespace rsp::security

#endif /* INCLUDE_SECURITY_SECUREALLOCATOR_H_ */
