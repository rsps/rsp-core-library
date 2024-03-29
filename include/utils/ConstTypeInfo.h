/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_CONSTTYPEINFO_H_
#define INCLUDE_UTILS_CONSTTYPEINFO_H_

#include <stdint.h>
#include <string_view>
#include "Crc32.h"
#include "Fnv1a.h"

namespace rsp::utils {


// https://stackoverflow.com/questions/35941045/can-i-obtain-c-type-names-in-a-constexpr-way/35943472#35943472
template<typename T>
constexpr std::string_view NameOf() noexcept
{
    #ifdef _MSC_VER
        #define __PRETTY_FUNCTION__ __FUNCSIG__
    #endif
    char const *p = __PRETTY_FUNCTION__;
    while (*p++ != '=')
        ;
    for (; *p == ' ' ; ++p)
        ;
    char const *p2 = p;
    char const *delim = p;
    int count = 1;
    for (; ; ++p2) {
        switch (*p2) {
            case ';':
                delim = p2;
                break;
            case '[':
                ++count;
                break;
            case ']':
                --count;
                if (!count) {
                    if (delim != p) {
                        return {p, std::size_t(delim - p)};
                    }
                    return {p, std::size_t(p2 - p)};
                }
                break;
            default:
                break;
        }
    }
    return {};
}

namespace fnv1a {

template<typename T>
constexpr uint32_t HashOf() noexcept
{
    return fnv1a::Hash32Const(NameOf<T>().data());
}

}

namespace crc32 {

template<typename T>
constexpr uint32_t HashOf() noexcept
{
    return crc32::HashConst(NameOf<T>().data());
}

} /* namespace crc32 */

struct TypeInfo
{
    std::string mName;
    union {
        uint32_t mId;
        char32_t mIdChar;
    };
};

template<typename T>
TypeInfo MakeTypeInfo() noexcept
{
    return {std::string(NameOf<T>()), crc32::HashOf<T>()};
}

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_CONSTTYPEINFO_H_ */
