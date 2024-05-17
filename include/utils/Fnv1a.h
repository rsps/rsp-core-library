/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_FNV1A_H
#define RSP_CORE_LIB_UTILS_FNV1A_H

#include <cstdint>
#include <string_view>
#include <string>

namespace rsp::utils {

class Fnv1a
{
public:
    static uint32_t Hash32(const void *apData, uint32_t aLen);
    static uint32_t Hash32(std::string_view aStr) { return Hash32(aStr.data(), aStr.size()); }
    static uint32_t Hash32(const char *apCStr) { return Hash32(std::string_view(apCStr)); }
    static uint32_t Hash32(const std::string &arStr) { return Hash32(arStr.data(), arStr.size()); }

    static uint64_t Hash64(const void *apData, uint64_t aLen);
    static uint64_t Hash64(std::string_view aStr) { return Hash64(aStr.data(), aStr.size()); }
    static uint64_t Hash64(const char *apCStr) { return Hash64(std::string_view(apCStr)); }
    static uint64_t Hash64(const std::string &arStr) { return Hash64(arStr.data(), arStr.size()); }
};


namespace fnv1a {
    // See: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    constexpr uint32_t cOffsetBasis32 = 2166136261u;
    constexpr uint32_t cPrime32 = 16777619u;
    constexpr uint64_t cOffsetBasis64 = 14695981039346656037UL;
    constexpr uint64_t cPrime64 = 1099511628211UL;

    inline constexpr uint32_t Hash32Const(const char* const str, const uint32_t value = cOffsetBasis32) noexcept // NOLINT, recursive call chain
    {
        return (*str == '\0') ? value : Hash32Const(str + 1, (value ^ static_cast<uint8_t>(*str)) * cPrime32);
    }

    inline constexpr uint64_t Hash64Const(const char* const str, const uint64_t value = cOffsetBasis64) noexcept // NOLINT, recursive call chain
    {
        return (*str == '\0') ? value : Hash64Const(str + 1, (value ^ static_cast<uint8_t>(*str)) * cPrime64);
    }
} /* namespace fnv1a */

size_t constexpr operator "" _fnv1a( const char* str, size_t /*len*/ )
{
    return fnv1a::Hash32Const(str);
}

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_FNV1A_H
