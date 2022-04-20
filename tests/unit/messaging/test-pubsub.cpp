/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <messaging/Publisher.h>
#include <messaging/Subscriber.h>
#include <messaging/eventTypes/ClickedEvent.h>
#include <doctest.h>

using namespace rsp::messaging;
enum testTopic {
    topicOne,
    topicTwo
};
class TestBroker : public Broker<testTopic>
{
  public:
    const char* GetName() { return __FUNCTION__; }
    std::map<int, std::vector<SubscriberBase *>>& GetSubscriberMap()
    {
        return mSubscriberMap;
    }
};

enum OtherTopic { other1, other2, other3 };
class OtherBroker : public Broker<OtherTopic>
{
public:
    const char* GetName() { return __FUNCTION__; }
    std::map<int, std::vector<SubscriberBase *>>& GetSubscriberMap()
    {
        return mSubscriberMap;
    }
};

class TestSubOne : public Subscriber<testTopic>
{
public:
    TestSubOne(Broker<testTopic>& arBroker)
        : Subscriber<testTopic>(arBroker)
    {
    }
    void HandleEvent(Event &arNewEvent)
    {
        isHandled = true;
        mMessage = arNewEvent.GetAs<ClickedEvent>().mMessage;
    }
    bool isHandled = false;
    std::string mMessage{};
};
class TestSubTwo : public Subscriber<testTopic>
{
public:
    TestSubTwo(Broker<testTopic>& arBroker)
        : Subscriber<testTopic>(arBroker)
    {
    }
    void HandleEvent(Event &arNewEvent)
    {
        isHandled = true;
        mMessage = arNewEvent.GetAs<ClickedEvent>().mMessage;
    }
    bool isHandled = false;
    std::string mMessage{};
};

TEST_CASE("Subscriber Test")
{
    // Arrange
    TestBroker testBroker;
    TestSubOne testSub(testBroker);
    ClickedEvent testEvent("MyEvent");

    SUBCASE("Subscribe to Broker")
    {
        // Act & Assert
        CHECK_NOTHROW(testSub.Subscribe(testTopic::topicOne));
        CHECK_NOTHROW(testSub.Subscribe(testTopic::topicTwo));
        CHECK(testBroker.GetSubscriberMap()[testTopic::topicOne].size() == 1);
        CHECK(testBroker.GetSubscriberMap()[testTopic::topicTwo].size() == 1);

        SUBCASE("Receive Event")
        {
            // Act
            testBroker.Publish(testTopic::topicOne, testEvent);

            // Assert
            CHECK(testSub.isHandled);
            CHECK_EQ(testSub.mMessage, "MyEvent");
        }
        SUBCASE("Unsubscribe to Broker through Broker")
        {
            // Act
            CHECK_NOTHROW(testBroker.Unsubscribe(testSub, testTopic::topicOne));
            CHECK(testBroker.GetSubscriberMap()[testTopic::topicOne].size() == 0);
            CHECK_NOTHROW(testBroker.Publish(testTopic::topicOne, testEvent));

            // Assert
            CHECK_FALSE(testSub.isHandled);

            CHECK_NOTHROW(testBroker.Publish(testTopic::topicTwo, testEvent));
            CHECK(testSub.isHandled);
            CHECK_EQ(testSub.mMessage, "MyEvent");
        }

        SUBCASE("Unsubscribe to Broker through Subscriber")
        {
            CHECK_NOTHROW(testSub.Unsubscribe(testTopic::topicOne));
            CHECK(testBroker.GetSubscriberMap()[testTopic::topicOne].size() == 0);
            CHECK_NOTHROW(testBroker.Publish(testTopic::topicOne, testEvent));
            CHECK_FALSE(testSub.isHandled);

            CHECK_NOTHROW(testBroker.Publish(testTopic::topicTwo, testEvent));
            CHECK(testSub.isHandled);
            CHECK_EQ(testSub.mMessage, "MyEvent");
        }

        SUBCASE("Unsubscribe all on destroy")
        {
            {
                TestSubTwo two(testBroker);
                CHECK_NOTHROW(two.Subscribe(testTopic::topicOne));
                CHECK_NOTHROW(two.Subscribe(testTopic::topicTwo));

                CHECK(testBroker.GetSubscriberMap()[testTopic::topicOne].size() == 2);
                CHECK(testBroker.GetSubscriberMap()[testTopic::topicTwo].size() == 2);

                CHECK_NOTHROW(testBroker.Publish(testTopic::topicOne, testEvent));
                CHECK(two.isHandled);
                CHECK_EQ(two.mMessage, "MyEvent");
            }
            CHECK(testBroker.GetSubscriberMap()[testTopic::topicOne].size() == 1);
            CHECK(testBroker.GetSubscriberMap()[testTopic::topicTwo].size() == 1);
        }
    }
}
TEST_CASE("Publisher Test")
{
    // Arrange
    TestBroker testBroker;
    TestSubOne testSubOne(testBroker);
    TestSubTwo testSubTwo(testBroker);
    testSubOne.Subscribe(testTopic::topicOne);
    testSubTwo.Subscribe(testTopic::topicTwo);
    Publisher<testTopic> myPub(testBroker);
    ClickedEvent testEvent("MyPublishedEvent");

    SUBCASE("Publish with correctly templated call")
    {
        // Act
        myPub.PublishToBroker(testTopic::topicOne, testEvent);

        // Assert
        CHECK(testSubOne.isHandled);
        CHECK_EQ(testSubOne.mMessage, "MyPublishedEvent");
        CHECK_FALSE(testSubTwo.isHandled);
    }

    // Is this the wanted behavior?
    /*
     * No, the PublishToBroker silently casts the wrongTopic into something acceptable by BrokerBase
     * thereby hiding the error, which may result in a valid event or an event with no subscribers.
     *
     * By calling directly through the broker, the error is detected at compile time:
     * testBroker.Publish(wrongTopic::wrongOne, testEvent);
     *
     * Fixed by turning Publisher into template class, to allow check for topic type.
     * Now the commented out code will not compile.
     */
    SUBCASE("Publish with incorrectly templated call")
    {
        // Arrange
        enum wrongTopic {
            wrongOne,
            wrongtwo
        };

        // Act
//        myPub.PublishToBroker(wrongTopic::wrongOne, testEvent);

        // Assert
        CHECK_FALSE(testSubOne.isHandled);
        CHECK_FALSE(testSubTwo.isHandled);
    }

    SUBCASE("Broker exchange") {
        OtherBroker otherBroker;
        // The following will not compile. Compile time type safety is working.
//        myPub.RegisterBroker(otherBroker);

//        myPub.PublishToBroker(OtherTopic::other1, testEvent);

        CHECK_FALSE(testSubOne.isHandled);
        CHECK_FALSE(testSubTwo.isHandled);
    }
}
