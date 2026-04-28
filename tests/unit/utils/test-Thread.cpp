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

#include <atomic>
#include <format>
#include <thread>

using namespace rsp::utils;

TEST_SUITE_BEGIN("Utils");

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
        catch (const std::runtime_error& e) {
            CHECK(StrUtils::StartsWith(e.what(), "Exception thrown in thread '"));
        }
        catch (...) {
            FAIL("Wrong exception was thrown");
        }
    }

    SUBCASE("Many Threads") {
        std::size_t num_threads_initial = ThreadList::GetInstance().GetThreadNames().size();

        // Stress-test Thread/ThreadList by concurrently adding and removing
        // threads from multiple threads.
        constexpr std::size_t num_workers = 8;
        constexpr std::size_t num_iterations = 100;
        std::atomic<bool> go{false};
        std::vector<std::jthread> workers;

        for (std::size_t i = 0; i < num_workers; ++i) {
            workers.emplace_back([&, i]() {
                // Spin until all workers until "go" so they start ~simultaneously
                while (!go.load(std::memory_order_acquire)) {
                    ;
                }

                for (std::size_t j = 0; j < num_iterations; ++j) {
                    Thread thrd{std::format("Thread #{}.{}", i, j)};                          // ctor adds thrd to ThreadList
                    [[maybe_unused]] auto names = ThreadList::GetInstance().GetThreadNames(); // force a read
                    // thrd's dtor removes it from ThreadList
                }
            });
        }

        // Unleash all workers
        go.store(true, std::memory_order_release);

        for (auto& w : workers) {
            w.join();
        }

        CHECK_EQ(ThreadList::GetInstance().GetThreadNames().size(), num_threads_initial);
    }
}

TEST_SUITE_END();
