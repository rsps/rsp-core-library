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

/**
 * \brief A string implementation that clears the memory of its content on destruction.
 */
class SecureString : public std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>
{
public:
    using std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>::basic_string;

    /**
     * \brief Construct with contents from a SecureBuffer.
     * \param arOther SecureBuffer to copy data from
     */
    SecureString(const SecureBuffer& arOther) // NOLINT, conversion constructor
        : std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>::basic_string()
    {
        assign(arOther.begin(), arOther.end());
    }

    /**
     * \brief Construct with contents from std::string
     * \param arOther string with contents to copy from
     */
    SecureString(const std::string& arOther) // NOLINT, conversion constructor
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

    /**
     * \brief Convert this to a std::string_view of the current content
     * \return string_view
     */
    operator std::string_view() const // NOLINT, conversion operator
    {
        return data();
    }

    /**
     * \brief Convert this to a std::string with the current content.
     * \return string
     */
    operator std::string() const // NOLINT, conversion operator
    {
        return data();
    }

    /**
     * \brief Convert this to a SecureBuffer with the current content.
     * \return SecureBuffer
     */
    operator SecureBuffer() const // NOLINT, conversion operator
    {
        return {reinterpret_cast<const std::uint8_t*>(data()), size()};
    }

    /**
     * \brief Assignment operator, assign from SecureBuffer
     * \param arOther SecureBuffer with data to copy
     * \return Reference to this
     */
    SecureString& operator=(const SecureBuffer& arOther)
    {
        std::stringstream ss;
        ss << arOther;
        assign(ss.str());
        return *this;
    }

    /**
     * \brief Equality operator, compares the content of this with another SecureString
     * \param arOther Other SecureString to compare with
     * \return
     */
    bool operator==(const SecureString& arOther)
    {
        return compare(arOther) == 0; // NOLINT
    }

    /**
     * \brief Equality operator, compares the content of this with a std::string
     * \param arOther std::string to compare with
     * \return
     */
    bool operator==(const std::string& arOther)
    {
        return compare(arOther) == 0; // NOLINT
    }

    /**
     * \brief Equality operator, compares the content of this with a zero delimited C style string
     * \param arOther Pointer to C style string to compare with
     * \return
     */
    bool operator==(const char* apOther)
    {
        return compare(apOther) == 0; // NOLINT
    }
};

} /* namespace rsp::security */

#endif /* INCLUDE_SECURITY_SECURESTRING_H_ */
