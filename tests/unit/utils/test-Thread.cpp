/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <utils/Thread.h>
#include <TestHelpers.h>

using namespace rsp::utils;

TEST_CASE("Threads")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    CHECK_NOTHROW(Thread t);

    Thread t("MyThread");

    SUBCASE("Idle Run") {
        CHECK_NOTHROW(t.Start());
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        CHECK_NOTHROW(t.Stop());
    }

    SUBCASE("Execute") {
        int count = 0;
        t.GetExecute() = [&](void) noexcept {
            count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        };
        CHECK_NOTHROW(t.Start());
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        CHECK_NOTHROW(t.Stop());
        CHECK(count > 10);
    }

    SUBCASE("Throw") {
        int count = 0;
        t.GetExecute() = [&](void) {
            throw std::runtime_error("Oh no!");
        };
        CHECK_NOTHROW(t.Start());
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        CHECK_THROWS_WITH_AS(t.Stop(), "Exception thrown in thread 'MyThread': Oh no!", std::runtime_error);
    }

}
