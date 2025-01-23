/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include "doctest.h"
#include <utils/PollTimer.h>
#include <thread>

using namespace rsp::utils;

TEST_CASE("PollTimer") {
    using namespace std::chrono_literals;

    PollTimer pt;
    PollTimer pt100(100);
    PollTimer pt200(200ms);

    CHECK(pt.IsExpired());
    CHECK_NOTHROW(pt.Set(300));
    CHECK_FALSE(pt100.IsExpired());
    CHECK_FALSE(pt200.IsExpired());

    std::this_thread::sleep_for(100ms);
    CHECK_FALSE(pt.IsExpired());
    CHECK(pt100.IsExpired());
    CHECK_NOTHROW(pt100.Set(100ms));
    CHECK_FALSE(pt100.IsExpired());
    CHECK_FALSE(pt200.IsExpired());

    std::this_thread::sleep_for(100ms);
    CHECK_FALSE(pt.IsExpired());
    CHECK(pt100.IsExpired());
    CHECK(pt200.IsExpired());

    std::this_thread::sleep_for(100ms);
    CHECK(pt.IsExpired());
}
