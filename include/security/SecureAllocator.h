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


template <typename T>
struct SecureAllocator: public std::allocator<T>
{
public:
    using std::allocator<T>::allocator;

    void cleanse(T* p, std::size_t n) {
//        std::cout << "Cleansing " << n << " bytes" << std::endl;
        std::memset(p, 0, n * sizeof(T));
    }

    void deallocate(T* p, std::size_t n) {
        cleanse(p, n);
        std::allocator<T>::deallocate(p, n);
    }
};


} // namespace rsp::security

#endif /* INCLUDE_SECURITY_SECUREALLOCATOR_H_ */
