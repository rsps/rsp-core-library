/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_SECURESTRING_H_
#define INCLUDE_SECURITY_SECURESTRING_H_

#include <string>
#include "SecureAllocator.h"

namespace rsp::security {

class SecureString : public std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>
{
public:
    using std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>::basic_string;

    ~SecureString()
    {
        auto n = size();
        if (n < 16) {
            get_allocator().cleanse(data(), n);
        }
    }
};

} /* namespace rsp::security */

#endif /* INCLUDE_SECURITY_SECURESTRING_H_ */
