/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_RANDOM_H
#define RSP_CORE_LIB_UTILS_RANDOM_H

#include <random>

namespace rsp::utils {

class Random
{
public:
    static void Seed(unsigned int aSeed)
    {
        Generator().seed(aSeed);
    }

    template<typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    static T Roll(T aMin, T aMax)
    {
        std::uniform_int_distribution<T> distribution(aMin, aMax);
        return distribution(Generator());
    }

    template<typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    static T Roll(T aMin, T aMax)
    {
        std::uniform_real_distribution<T> distribution(aMin, aMax);
        return distribution(Generator());
    }

    static std::default_random_engine& Generator()
    {
        static std::default_random_engine generator{};
        return generator;
    }
};

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_RANDOM_H
