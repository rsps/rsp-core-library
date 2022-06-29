/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/Framebuffer.h>
#include <graphics/controls/Scene.h>
#include <messaging/Subscriber.h>
#include <messaging/Publisher.h>
#include <posix/FileSystem.h>
#include <doctest.h>

#include <eventTypes/ClickedEvent.h>
#include <scenes/Scenes.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::messaging;

class TestSub : public Subscriber<ClickTopics>
{
  public:
    TestSub(Broker<ClickTopics>& arBroker)
        : Subscriber<ClickTopics>(arBroker)
    {
    }

    void HandleEvent(Event &arNewEvent) override
    {
        auto event = arNewEvent.GetAs<rsp::messaging::ClickedEvent>();

        message = event.mMessage;
        calledCount++;
    }
    int calledCount = 0;
    std::string message{};
};


TEST_CASE("Scene Test")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    // Arrange
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    Scenes scenes;
    TouchEvent event;

    Rect tr = scenes.Second().GetTopArea().GetArea();
    Point insideTopPoint(tr.GetLeft() + (rand() % tr.GetWidth()),
                         tr.GetTop() + (rand() % tr.GetHeight()));

    Rect br = scenes.Second().GetBotArea().GetArea();
    Point insideBotPoint(br.GetLeft() + (rand() % br.GetWidth()),
                         br.GetTop() + (rand() % br.GetHeight()));

    SUBCASE("Scene Process Input")
    {
        // Arrange
        event.mType = TouchEvent::Types::Press;
        scenes.Second().Render(fb);

        SUBCASE("Process input for Top elements")
        {
            // Arrange
            event.mPoint = insideTopPoint;
            MESSAGE("Event Point:" << event.mPoint);

            // Act
            scenes.Second().ProcessInput(event);

            // Assert
            CHECK(scenes.Second().GetTopImg().IsInvalid());
            CHECK_FALSE(scenes.Second().GetBotImg().IsInvalid());
        }
        SUBCASE("Process input for Bot elements")
        {
            // Arrange
            event.mPoint = insideBotPoint;

            // Act
            scenes.Second().ProcessInput(event);

            // Assert
            CHECK(scenes.Second().GetBotImg().IsInvalid());
            CHECK_FALSE(scenes.Second().GetTopImg().IsInvalid());
        }
    }
    SUBCASE("Scene Render Elements")
    {
        // Arrange
        scenes.Second().GetTopImg().Invalidate();
        scenes.Second().GetBotImg().Invalidate();
        CHECK(scenes.Second().GetTopImg().IsInvalid());
        CHECK(scenes.Second().GetBotImg().IsInvalid());

        // Act
        scenes.Second().Render(fb);

        // Assert
        CHECK_FALSE(scenes.Second().GetTopImg().IsInvalid());
        CHECK_FALSE(scenes.Second().GetBotImg().IsInvalid());
    }

    SUBCASE("Scene Bind click Callbacks")
    {
        Broker<ClickTopics> broker;
        Publisher<ClickTopics> publisher(broker);
        bool clicked = false;
        scenes.Second().Whenclicked() = [&publisher, &clicked]() {
            clicked = true;
            rsp::messaging::ClickedEvent event("Button was clicked.");
            publisher.PublishToBroker(ClickTopics::SceneChange, event);
        };

        // Arrange
        TestSub sub(broker);
        sub.Subscribe(ClickTopics::SceneChange);
        event.mPoint = insideBotPoint;

        // Act
        event.mType = TouchEvent::Types::Press;
        scenes.Second().ProcessInput(event);
        event.mType = TouchEvent::Types::Lift;
        scenes.Second().ProcessInput(event);

         // Assert
        CHECK(clicked);
        CHECK(sub.calledCount == 1);
        CHECK(sub.message == "Button was clicked.");
    }
}
