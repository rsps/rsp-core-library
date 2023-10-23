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
#include <messaging/Event.h>
#include <messaging/EventBroker.h>
#include <TestHelpers.h>

using namespace rsp::messaging;

static int event1_count = 0;
static int event2_count = 0;

class TestEvent1 : public EventBase<TestEvent1>
{
};

class TestEvent2 : public EventBase<TestEvent2>
{
public:
    explicit TestEvent2(int aValue = 1) : mValue(aValue) {}
    int mValue;
};

class NotAnEvent_ShouldNotCompile
{
};

class Subscriber : public SubscriberInterface
{
public:
    bool ProcessEvent(rsp::messaging::Event &arEvent) override
    {
        switch (arEvent.Type) {
            case TestEvent1::ClassType:
                event1_count++;
                break;

            case TestEvent2::ClassType:
                event2_count += arEvent.CastTo<TestEvent2>().mValue;
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
        EventBroker dummy; // Create first instance, which is automatically registered as default instance
        EventBroker::GetInstance(); // Check for default instance
    );
    CHECK_THROWS_AS(EventBroker::GetInstance(), rsp::exceptions::ENoInstance); // Check that default instance has disappeared.

    EventBroker broker;
    BrokerInterface &eb = broker;

    Subscriber sub1;
    Subscriber sub2;

//    eb.Publish<NotAnEvent_ShouldNotCompile>();

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

    CHECK_EQ(event1_count, 4);
    CHECK_EQ(event2_count, 42);
}

