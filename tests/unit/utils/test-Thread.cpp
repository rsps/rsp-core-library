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
#include <utils/StrUtils.h>
#include <utils/Thread.h>
#include <utils/ThreadList.h>
#include <TestHelpers.h>

using namespace rsp::utils;

TEST_CASE("Threads")
{
    TestLogger logger;
    ThreadList thread_list;

    CHECK_NOTHROW(Thread t("Dummy"));
    CHECK_THROWS_AS(Thread t(""), ThreadException);

    CHECK_THROWS_AS(thread_list.GetThreadByName("MyThread"), rsp::exceptions::NotSetException);

    Thread t("MyThread");

    CHECK_EQ(ThreadList::GetInstance().GetThreadNames()[0], "MyThread");
    CHECK_NOTHROW(thread_list.GetThreadByName("MyThread"));

    SUBCASE("Idle Run") {
        CHECK_NOTHROW(t.Start());
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        CHECK_NOTHROW(t.Stop());
    }

    SUBCASE("Execute") {
        int count = 0;
        t.SetExecute([&]() noexcept {
            count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        });
        CHECK_NOTHROW(t.Start());
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        CHECK_NOTHROW(t.Stop());
        CHECK_GT(count, 10);
    }

    SUBCASE("Throw") {
        t.SetExecute([&]() {
            throw std::runtime_error("Oh no!");
        });
        CHECK_NOTHROW(t.Start());
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        CHECK_THROWS_AS(t.Stop(), std::runtime_error);
    }

    SUBCASE("Throw NoName") {
        CHECK_NE(t.GetName(), "");

        t.SetExecute([&]() {
            throw std::runtime_error("Oh no!");
        });

        CHECK_NOTHROW(t.Start());
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        try {
            t.Stop();
            FAIL("No exception was thrown");
        }
        catch(const std::runtime_error &e) {
            CHECK(StrUtils::StartsWith(e.what(), "Exception thrown in thread '"));
        }
        catch(...) {
            FAIL("Wrong exception was thrown");
        }
    }

}
