/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <string>
#include <doctest.h>
#include <messaging/Notifier.h>

using namespace rsp::messaging;

static std::string s_param = "Hello World";
static int checker_called = 0;

static void checker(int i, const std::string& s)
{
    checker_called++;
    CHECK_EQ(i, 42);
    CHECK_EQ(s, s_param);
}

TEST_CASE("Emitter")
{
    using Notifier = Notifier<int, const std::string&>;

    SUBCASE("Constructors") {
        CHECK_NOTHROW(Notifier dummy);
        CHECK_NOTHROW(Notifier::Listener_t dummy;);
        CHECK_NOTHROW(
            {
                Notifier emitter;
                Notifier::Listener_t sub = emitter.Listen([&](int, const std::string&) noexcept {});
            }
        );
    }

    SUBCASE("Single") {
        Notifier emitter;
        checker_called = 0;
        Notifier::Listener_t subscriber3 = emitter.Listen(checker);
        CHECK(bool(subscriber3));
        CHECK_NOTHROW(emitter(42, s_param));
        CHECK_EQ(checker_called, 1);
    }

    SUBCASE("Attach") {
        Notifier emitter;
        checker_called = 0;
        Notifier::Listener_t subscriber3 = emitter.Listen(checker);
        CHECK(bool(subscriber3));
        CHECK_NOTHROW(emitter(42, s_param));
        CHECK_EQ(checker_called, 1);
        subscriber3 = nullptr;
        CHECK_FALSE(bool(subscriber3));
        CHECK_NOTHROW(emitter(42, s_param));
        CHECK_EQ(checker_called, 1);
        subscriber3 = emitter.Listen(checker);
        CHECK(bool(subscriber3));
        CHECK_NOTHROW(emitter(42, s_param));
        CHECK_EQ(checker_called, 2);
    }

    SUBCASE("Multi") {
        Notifier emitter;

        int sub1_called = 0;
        int sub2_called = 0;
        checker_called = 0;

        Notifier::Listener_t subscriber1 = emitter.Listen([&](int i, const std::string &s) {
            sub1_called++;
            CHECK_EQ(i, 42);
            CHECK_EQ(s, s_param);
        });

        CHECK(bool(subscriber1));
        CHECK_NOTHROW(emitter(42, s_param));
        CHECK_EQ(sub1_called, 1);

        Notifier::Listener_t subscriber2 = emitter.Listen([&](int i, const std::string &s) {
            sub2_called++;
            CHECK_EQ(i, 42);
            CHECK_EQ(s, s_param);
        });
        CHECK(bool(subscriber2));

        Notifier::Listener_t subscriber3 = emitter.Listen(checker);
        CHECK(bool(subscriber3));

        CHECK_NOTHROW(emitter(42, s_param));

        CHECK_EQ(sub1_called, 2);
        CHECK_EQ(sub2_called, 1);
        CHECK_EQ(checker_called, 1);

        CHECK_NOTHROW(subscriber2 = nullptr);

        CHECK_NOTHROW(emitter(42, s_param));

        CHECK_EQ(sub1_called, 3);
        CHECK_EQ(sub2_called, 1);
        CHECK_EQ(checker_called, 2);

        CHECK_NOTHROW(subscriber3 = nullptr);

        CHECK_NOTHROW(emitter(42, s_param));

        CHECK_EQ(sub1_called, 4);
        CHECK_EQ(sub2_called, 1);
        CHECK_EQ(checker_called, 2);
    }
}



