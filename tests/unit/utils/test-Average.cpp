/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "doctest.h"
#include <utils/Average.h>

TEST_SUITE_BEGIN("Utils");

TEST_CASE("Average") {
    using namespace rsp::utils;

    SUBCASE("Uint32") {
        Average<uint32_t, 10> avr;

        CHECK_THROWS_AS(avr.Get(), rsp::exceptions::AssertException);
        CHECK_THROWS_AS(static_cast<uint32_t>(avr), rsp::exceptions::AssertException);

        CHECK_EQ(avr.Add(2), 2);
        CHECK_EQ(avr.Add(4), 3);
        CHECK_EQ(avr.Add(2), 2);
        CHECK_EQ(avr.Sum(), 8);
        CHECK_EQ(avr.Count(), 3);
        CHECK_EQ(avr.Get(), 2);
        CHECK_EQ(static_cast<uint32_t>(avr), 2);

        CHECK_EQ(avr.Add(4), 3);
        CHECK_EQ(avr.Add(3), 3);
        CHECK_EQ(avr.Add(3), 3);
        CHECK_EQ(avr.Sum(), 18);
        CHECK_EQ(avr.Get(), 3);

        CHECK_EQ(avr.Add(10), 4);
        CHECK_EQ(avr.Add(4), 4);
        CHECK_EQ(avr.Add(4), 4);
        CHECK_EQ(avr.Add(14), 5);
        CHECK_EQ(avr.Sum(), 50);
        CHECK_EQ(avr.Count(), 10);
        CHECK_EQ(avr.Get(), 5);

        CHECK_EQ(avr.Add(0), 4);
        CHECK_EQ(avr.Add(0), 4);
        CHECK_EQ(avr.Add(0), 4);
        CHECK_EQ(avr.Add(0), 3);
        CHECK_EQ(avr.Sum(), 38);
        CHECK_EQ(avr.Count(), 10);
        CHECK_EQ(avr.Get(), 3);
    }

    SUBCASE("Float") {
        Average<float, 4> avr;

        CHECK_EQ(avr.Add(2.0f), 2.0f);
        CHECK_EQ(avr.Add(4.0f), 3.0f);
        CHECK_EQ(avr.Add(3.0f), 3.0f);
        CHECK_EQ(avr.Add(3.0f), 3.0f);
        CHECK_EQ(avr.Sum(), 12.0f);
        CHECK_EQ(avr.Count(), 4);
        CHECK_EQ(avr.Get(), 3.0f);
        CHECK_EQ(static_cast<float>(avr), 3.0f);

        CHECK_EQ(avr.Add(-4.0f), 1.5f);
        CHECK_EQ(avr.Add(-4.0f), -0.5f);
        CHECK_EQ(avr.Sum(), -2.0f);
        CHECK_EQ(avr.Count(), 4);
        CHECK_EQ(avr.Get(), -0.5f);

        avr.Clear();
        CHECK_EQ(avr.Count(), 0);
        CHECK_EQ(avr.Sum(), 0.0f);
        CHECK_EQ(avr.Add(12.0f), 12.0f);
        CHECK_EQ(avr.Add(6.0f), 9.0f);
        CHECK_EQ(avr.Sum(), 18.0f);
        CHECK_EQ(avr.Count(), 2);
        CHECK_EQ(avr.Get(), 9.0f);
    }
}

TEST_SUITE_END();
