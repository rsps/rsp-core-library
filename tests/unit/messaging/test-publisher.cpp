/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "messaging/Publisher.h"
#include "messaging/Subscriber.h"
#include <doctest.h>

using namespace rsp::messaging;

class TestSubOne : public Subscriber
{
  public:
    void HandleEvent(Event &arNewEvent)
    {
        isHandled = true;
    }
    bool isHandled = false;
};
class TestSubTwo : public Subscriber
{
  public:
    void HandleEvent(Event &arNewEvent)
    {
        isHandled = true;
    }
    bool isHandled = false;
};

TEST_CASE("Publisher Test")
{
    // Arrange
    enum testTopic {
        topicOne,
        topicTwo
    };
    Broker<testTopic> testBroker;
    TestSubOne testSubOne;
    TestSubTwo testSubTwo;
    testSubOne.SubscribeToBroker(testBroker, testTopic::topicOne);
    testSubTwo.SubscribeToBroker(testBroker, testTopic::topicTwo);
    Publisher myPub(testBroker);
    Event testEvent;

    SUBCASE("Publish with correctly templated call")
    {
        // Act
        myPub.PublishToBroker<testTopic>(testTopic::topicOne, testEvent);

        // Assert
        CHECK(testSubOne.isHandled);
        CHECK_FALSE(testSubTwo.isHandled);
    }

    // Is this the wanted behavior?
    SUBCASE("Publish with incorrectly templated call")
    {
        // Arrange
        enum wrongTopic {
            wrongOne,
            wrongtwo
        };

        // Act
        myPub.PublishToBroker<wrongTopic>(wrongTopic::wrongOne, testEvent);

        // Assert
        CHECK(testSubOne.isHandled);
        CHECK_FALSE(testSubTwo.isHandled);
    }
}