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
            assign(s);
        }
    }

    explicit FixedString(const std::string &arString)
        : FixedString(arString.data())
    {
    }

    FixedString<N>& operator=(const char *apS)
    {
        assign(apS);
        return *this;
    }

    FixedString<N>& operator=(const std::string &arOther)
    {
        assign(arOther.c_str());
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
    // Intentionally truncates to N chars; a manual loop avoids strncpy/strnlen
    // -Wstringop-truncation and -Wstringop-overread false positives.
    void assign(const char *apS)
    {
        std::size_t i = 0;
        for (; i < N && apS[i] != '\0'; ++i) {
            _mStr[i] = apS[i];
        }
        _mStr[i] = '\0';
    }

    char _mStr[N + 1]{}; // keep an extra char for zero terminator
} __attribute__((packed));

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_FIXED_STRING_H
