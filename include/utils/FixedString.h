/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_FIXED_STRING_H
#define RSP_CORE_LIB_UTILS_FIXED_STRING_H

#include <string>
#include <cstring>

namespace rsp::utils {

/**
 * \class FixedString
 * \brief Generic char[N+1] array wrapper with useful string operations.
 *
 * Useful C++ replacement for char[] members in data struct's.
 * For a generic fixed size buffer, use std::array.
 *
 * \tparam N
 */
template<std::size_t N>
class FixedString
{
public:

    FixedString(const char *s = nullptr) // NOLINT, Conversion constructor
    {
        _mStr[0] = _mStr[N] = 0;
        if (s) {
            std::strncpy(_mStr, s, N);
        }
    }

    explicit FixedString(const std::string &arString)
        : FixedString(arString.data())
    {
    }

    FixedString<N>& operator=(const char *apS)
    {
        std::strncpy(_mStr, apS, N);
        _mStr[N] = 0;
        return *this;
    }

    FixedString<N>& operator=(const std::string &arOther)
    {
        std::strncpy(_mStr, arOther.data(), N);
        _mStr[N] = 0;
        return *this;
    }

    explicit operator const char*() const
    {
        return _mStr;
    }

    operator std::string() const // NOLINT, Conversion operator
    {
        return _mStr;
    }

    bool operator==(const std::string &arS) const
    {
        return std::strncmp(_mStr, arS.c_str(), N+1) == 0;
    }

    bool operator==(const char *apS) const
    {
        return std::strncmp(_mStr, apS, N+1) == 0;
    }

    [[nodiscard]] std::size_t size() const
    {
        return std::strlen(&_mStr[0]);
    }

private:
    char _mStr[N + 1]{}; // keep an extra char for zero terminator
} __attribute__((packed));

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_FIXED_STRING_H
