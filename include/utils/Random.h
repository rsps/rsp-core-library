/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_RANDOM_H
#define RSP_CORE_LIB_RANDOM_H

#include <random>

namespace rsp::utils {

class Random
{
public:
    static void Seed(unsigned int aSeed)
    {
        mGenerator.seed(aSeed);
    }

    template<typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    static T Roll(T aMin, T aMax)
    {
        std::uniform_int_distribution<T> distribution(aMin, aMax);
        return distribution(mGenerator);
    }

    template<typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    static T Roll(T aMin, T aMax)
    {
        std::uniform_real_distribution<T> distribution(aMin, aMax);
        return distribution(mGenerator);
    }

protected:
    static std::default_random_engine mGenerator;
};

std::default_random_engine Random::mGenerator{};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_RANDOM_H
