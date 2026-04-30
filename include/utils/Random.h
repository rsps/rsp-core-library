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

#include <concepts>
#include <random>

namespace rsp::utils {

class Random
{
public:
    using Engine = std::default_random_engine;

    static void Seed(const Engine::result_type aSeed)
    {
        Generator().seed(aSeed);
    }

    template <std::integral T>
    static T Roll(T aMin, T aMax)
    {
        std::uniform_int_distribution<T> distribution(aMin, aMax);
        return distribution(Generator());
    }

    template <std::floating_point T>
    static T Roll(T aMin, T aMax)
    {
        std::uniform_real_distribution<T> distribution(aMin, aMax);
        return distribution(Generator());
    }

    static Engine& Generator()
    {
        std::random_device r;
        static Engine generator(r());
        return generator;
    }
};

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_RANDOM_H
