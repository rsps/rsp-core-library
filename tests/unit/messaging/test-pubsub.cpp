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
enum testTopic {
    topicOne,
    topicTwo
};
class TestBroker : public Broker<testTopic>
{
  public:
    std::map<int, std::vector<Subscriber *>> &GetSubscriberMap()
    {
        return mSubscriberMap;
    }
};

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

TEST_CASE("Subscriber Test")
{
    // Arrange
    TestBroker testBroker;
    TestSubOne testSub;
    Event testEvent;

    SUBCASE("Subscribe to Broker")
    {
        // Act & Assert
        CHECK_NOTHROW(testSub.SubscribeToBroker(testBroker, testTopic::topicOne););
        CHECK(testBroker.GetSubscriberMap()[testTopic::topicOne].size() == 1);

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
            CHECK(testBroker.GetSubscriberMap()[testTopic::topicOne].size() == 0);
            CHECK_NOTHROW(testBroker.Publish(testTopic::topicOne, testEvent););

            // Assert
            CHECK_FALSE(testSub.isHandled);
        }
        /*SUBCASE("Unsubscribe to Broker through Subscriber")
        {

        }*/
    }
}
TEST_CASE("Publisher Test")
{
    // Arrange
    TestBroker testBroker;
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