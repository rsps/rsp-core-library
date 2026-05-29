/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <rsp/messaging/Event.h>
#include <rsp/messaging/EventBroker.h>
#include <TestHelpers.h>

using namespace rsp::messaging;

TEST_SUITE_BEGIN("Messaging");

struct TestEvent1 : public EventBase<TestEvent1>
{
    static int event_count;
};

class TestEvent2 : public EventBase<TestEvent2>
{
public:
    static int event_count;
    explicit TestEvent2(const int aValue = 1) : mValue(aValue) {}
    int mValue;
};

int TestEvent1::event_count = 0;
int TestEvent2::event_count = 0;

class NotAnEvent_ShouldNotCompile
{
};

class Subscriber : public SubscriberInterface
{
public:
    bool ProcessEvent(Event &arEvent) override
    {
        switch (arEvent.Type) {
            case TestEvent1::ClassType:
                TestEvent1::event_count++;
                break;

            case TestEvent2::ClassType:
                TestEvent2::event_count += arEvent.CastTo<TestEvent2>().mValue;
                break;

            default:
                break;
        }
        return false;
    }
};

TEST_CASE("EventBroker")
{
    TestLogger logger;

    CHECK_THROWS_AS(EventBroker::GetInstance(), rsp::exceptions::ENoInstance);
    CHECK_NOTHROW(
        EventBroker dummy; // Create the first instance, which is automatically registered as the default instance
        EventBroker::GetInstance(); // Check for the default instance
    );
    CHECK_THROWS_AS(EventBroker::GetInstance(), rsp::exceptions::ENoInstance); // Check that the default instance has disappeared.

    EventBroker broker;
    BrokerInterface &eb = broker;

    Subscriber sub1;
    Subscriber sub2;

    // eb.Publish<NotAnEvent_ShouldNotCompile>(); // NOSONAR

    CHECK_NOTHROW(eb.Publish<TestEvent1>()); // event1_count +0
    CHECK_NOTHROW(eb.ProcessEvents());

    CHECK_NOTHROW(EventBroker::GetInstance().Subscribe(sub1));

    CHECK_NOTHROW(eb.Publish<TestEvent1>()); // event1_count +1
    CHECK_NOTHROW(eb.ProcessEvents());

    CHECK_NOTHROW(eb.Subscribe(sub2));

    CHECK_NOTHROW(eb.Publish<TestEvent1>()); // event1_count +2
    CHECK_NOTHROW(eb.Publish<TestEvent2>()); // event2_count +2
    CHECK_NOTHROW(eb.ProcessEvents());

    CHECK_NOTHROW(eb.Unsubscribe(sub1));

    CHECK_NOTHROW(eb.Publish<TestEvent1>());   // event1_count +1
    CHECK_NOTHROW(eb.Publish<TestEvent2>(40)); // event2_count +40
    CHECK_NOTHROW(eb.ProcessEvents());

    CHECK_EQ(TestEvent1::event_count, 4);
    CHECK_EQ(TestEvent2::event_count, 42);
}

TEST_SUITE_END();
