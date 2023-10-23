/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/Fnv1a.h>

namespace rsp::utils {

std::uint32_t Fnv1a::Hash32(const void *apData, std::uint32_t aLen)
{
    uint32_t hash = fnv1a::cOffsetBasis32;
    uint32_t prime = fnv1a::cPrime32;

    const auto *data = reinterpret_cast<const uint8_t *>(apData);

    for(uint32_t i = 0; i < aLen; ++i) {
        hash = (hash ^ data[i]) * prime;
    }

    return hash;
}

std::uint64_t Fnv1a::Hash64(const void *apData, std::uint64_t aLen)
{
    uint64_t hash = fnv1a::cOffsetBasis64;
    uint64_t prime = fnv1a::cPrime64;

    const auto *data = reinterpret_cast<const uint8_t *>(apData);

    for(uint32_t i = 0; i < aLen; ++i) {
        hash = (hash ^ data[i]) * prime;
    }

    return hash;
}

} /* namespace rsp::utils */
