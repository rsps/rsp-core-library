/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "messaging/Subscriber.h"
#include <doctest.h>

using namespace rsp::messaging;

class TestSub : public Subscriber
{
  public:
    void HandleEvent(Event &arNewEvent)
    {
        isHandled = true;
    }
    bool isHandled = false;
};
enum testTopic {
    topicOne,
    topicTwo
};

TEST_CASE("Subscriber Test")
{
    // Arrange
    Broker<testTopic> testBroker;
    TestSub testSub;
    Event testEvent;

    SUBCASE("Subscribe to Broker")
    {
        // Act & Assert
        CHECK_NOTHROW(testSub.SubscribeToBroker(testBroker, testTopic::topicOne););

        SUBCASE("Recieve Event")
        {
            // Act
            testBroker.Publish(testTopic::topicOne, testEvent);

            // Assert
            CHECK(testSub.isHandled);
        }
        SUBCASE("Unsubscribe to Broker through Broker")
        {
            // Act
            CHECK_NOTHROW(testBroker.RemoveSubscriber(testSub, testTopic::topicOne););
            CHECK_NOTHROW(testBroker.Publish(testTopic::topicOne, testEvent););

            // Assert
            CHECK_FALSE(testSub.isHandled);
        }
        SUBCASE("Unsubscribe to Broker through Subscriber")
        {
            // Act
            delete &testSub;
            TestSub testSub;
            CHECK_NOTHROW(testBroker.Publish(testTopic::topicOne, testEvent););

            // Assert
            CHECK_FALSE(testSub.isHandled);
        }
    }
}