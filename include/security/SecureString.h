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
#include <sstream>
#include "SecureAllocator.h"
#include "SecureBuffer.h"

namespace rsp::security {

class SecureString : public std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>
{
public:
    using std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>::basic_string;

    SecureString(const SecureBuffer& arOther)
        : std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>::basic_string()
    {
        assign(arOther.begin(), arOther.end());
    }

    SecureString(const std::string& arOther)
        : std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>::basic_string()
    {
        assign(arOther);
    }

    /**
     * \fn  ~SecureString()
     * \brief Custom destructor that clear memory for short strings
     *
     * Because of "C++ small string optimization" std::basic_string does not allocate
     * memory for strings smaller than 16 bytes, so SecureAllocator is never invoked.
     * In case of a short string we call the cleanse method directly from this
     * destructor.
     *
     * \see https://tc-imba.github.io/posts/cpp-sso
     */
    ~SecureString()
    {
        auto n = size();
        if (n < 16) {
            get_allocator().cleanse(data(), n);
        }
    }

    operator std::string_view()
    {
        return data();
    }

    SecureString& operator=(const SecureBuffer& arOther)
    {
        std::stringstream ss;
        ss << arOther;
        assign(ss.str());
        return *this;
    }

    bool operator==(const SecureString& arOther)
    {
        return compare(arOther) == 0;
    }

    bool operator==(const std::string& arOther)
    {
        return compare(arOther) == 0;
    }

    bool operator==(const char* apOther)
    {
        return compare(apOther) == 0;
    }
};

} /* namespace rsp::security */

#endif /* INCLUDE_SECURITY_SECURESTRING_H_ */
