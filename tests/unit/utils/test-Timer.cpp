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
#include <chrono>
#include <thread>
#include <utils/Timer.h>
#include <utils/Random.h>

using namespace rsp::utils;
using namespace std::literals::chrono_literals;

TEST_CASE("Timer")
{
    CHECK_NOTHROW(Random::Seed(static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count())));

    bool triggered[2] = {false , false};
    int triggered1_at = 0;
    int triggered2_count = 0;
    int loop_count = 0;

    CHECK_NOTHROW(TimerQueue::Create());

    CHECK_NOTHROW(Timer tmp);
    Timer t1;
    Timer t2;

    CHECK_NOTHROW(t1.SetId(1));
    CHECK_NOTHROW(t1.SetTimeout(50ms));

    CHECK_NOTHROW(t2.SetId(2));
    CHECK_NOTHROW(t2.SetTimeout(20ms));

    t1.Callback() = [&](Timer& arTimer) {
        CHECK_EQ(&arTimer, &t1);
        CHECK_EQ(arTimer.GetId(), 1);
        triggered[0] = true;
        triggered1_at = loop_count;
    };

    t2.Callback() = [&](Timer& arTimer) {
        CHECK_EQ(&arTimer, &t2);
        CHECK_EQ(arTimer.GetId(), 2);
        CHECK_NOTHROW(arTimer.Enable());
        triggered[1] = true;
        triggered2_count++;
    };

    CHECK_FALSE(t1.IsEnabled());
    CHECK_NOTHROW(t1.Enable());
    CHECK(t1.IsEnabled());

    CHECK_FALSE(t2.IsEnabled());
    CHECK_NOTHROW(t2.Enable());
    CHECK(t2.IsEnabled());

    for (; loop_count < 20 ; ++loop_count) {
        std::this_thread::sleep_for(5ms);
        CHECK_NOTHROW(TimerQueue::Get().Poll());
    }

    CHECK(triggered[0]);
    CHECK(triggered[1]);
    CHECK_EQ(triggered1_at, 9);
    CHECK_EQ(t1.GetTimeout(), 50ms);
    CHECK_EQ(triggered2_count, 5);
    CHECK_EQ(t2.GetTimeout(), 20ms);

//    CHECK_NOTHROW(TimerQueue::Destroy());
}

