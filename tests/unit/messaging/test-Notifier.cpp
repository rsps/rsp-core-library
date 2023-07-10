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

static void checker(int i, std::string s)
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
        CHECK_NOTHROW(Notifier::ListenerType dummy;);
        CHECK_NOTHROW(Notifier::ListenerType dummy([&](int, const std::string&) noexcept {}));
        CHECK_NOTHROW(
            {
                Notifier emitter;
                Notifier::ListenerType sub(emitter, [&](int, const std::string&) noexcept {});
            }
        );
    }

    SUBCASE("Single") {
        Notifier emitter;
        checker_called = 0;
        Notifier::ListenerType subscriber3(emitter, checker);
        CHECK_NOTHROW(emitter.Emit(42, s_param));
        CHECK_EQ(checker_called, 1);
    }

    SUBCASE("Attach") {
        Notifier emitter;
        checker_called = 0;
        Notifier::ListenerType subscriber3(checker);
        CHECK_NOTHROW(emitter.Emit(42, s_param));
        CHECK_EQ(checker_called, 0);
        subscriber3.Attach(emitter);
        CHECK_NOTHROW(emitter.Emit(42, s_param));
        CHECK_EQ(checker_called, 1);
        subscriber3.Detach();
        CHECK_NOTHROW(emitter.Emit(42, s_param));
        CHECK_EQ(checker_called, 1);
    }

    SUBCASE("Multi") {
        Notifier emitter;

        int sub1_called = 0;
        int sub2_called = 0;
        checker_called = 0;

        Notifier::ListenerType subscriber1(emitter, [&](int i, const std::string &s) {
            sub1_called++;
            CHECK_EQ(i, 42);
            CHECK_EQ(s, s_param);
        });

        CHECK_NOTHROW(emitter.Emit(42, s_param));
        CHECK_EQ(sub1_called, 1);

        Notifier::ListenerType subscriber2(emitter, [&](int i, const std::string &s) {
            sub2_called++;
            CHECK_EQ(i, 42);
            CHECK_EQ(s, s_param);
        });

        Notifier::ListenerType subscriber3(emitter, checker);

        CHECK_NOTHROW(emitter.Emit(42, s_param));

        CHECK_EQ(sub1_called, 2);
        CHECK_EQ(sub2_called, 1);
        CHECK_EQ(checker_called, 1);

        CHECK_NOTHROW(emitter.Unsubscribe(&subscriber2));

        CHECK_NOTHROW(emitter.Emit(42, s_param));

        CHECK_EQ(sub1_called, 3);
        CHECK_EQ(sub2_called, 1);
        CHECK_EQ(checker_called, 2);

        CHECK_NOTHROW(subscriber2.Attach(emitter));
        CHECK_NOTHROW(subscriber3.Detach());

        CHECK_NOTHROW(emitter.Emit(42, s_param));

        CHECK_EQ(sub1_called, 4);
        CHECK_EQ(sub2_called, 2);
        CHECK_EQ(checker_called, 2);
    }
}



