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
#include <cstring>
#include <iostream>

namespace rsp::security {

/**
 * \brief A template implementation of a secure allocator. Memory is always cleared when freed.
 * \tparam T The element type to use by the allocator
 */
template <typename T>
struct SecureAllocator: public std::allocator<T>
{
public:
    using std::allocator<T>::allocator;

    /**
     * \brief Helper method to clear a memory range
     * \param p
     * \param n
     */
    void cleanse(T* p, std::size_t n) {
        std::memset(p, 0, n * sizeof(T));
    }

    /**
     * \brief Override to clear the memory before deallocation.
     * \param p
     * \param n
     */
    void deallocate(T* p, std::size_t n) {
        cleanse(p, n);
        std::allocator<T>::deallocate(p, n);
    }
};


} // namespace rsp::security

#endif /* INCLUDE_SECURITY_SECUREALLOCATOR_H_ */
