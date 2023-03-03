/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/SW/SWRenderer.h>
#include <graphics/Font.h>
#include <graphics/Scene.h>
#include <messaging/Subscriber.h>
#include <messaging/Publisher.h>
#include <posix/FileSystem.h>
#include <utils/Random.h>
#include <doctest.h>

#include <eventTypes/ClickedEvent.h>
#include <scenes/Scenes.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::messaging;
using namespace rsp::utils;

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

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Scene Test")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
    const char* cFontName = "Exo 2";

    CHECK_NOTHROW(Font::RegisterFont(cFontFile));
    CHECK_NOTHROW(Font::SetDefaultFont(cFontName));

    // Arrange
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    SWRenderer renderer(p);

    CHECK_NOTHROW(Scenes scenes_dummy);
    Scenes scenes;

    CHECK_NOTHROW(GfxEvent event_dummy);
    GfxEvent event;

    scenes.GetAfterCreate() = [](Scene *apScene) {
        CHECK_EQ(apScene->GetId(), SecondScene::ID);
        MESSAGE("Created Scene: " << apScene->GetName());
    };

    scenes.GetBeforeDestroy() = [](Scene *apScene) {
        MESSAGE("Destroying Scene: " << apScene->GetName());
    };

    scenes.SetActiveScene(SecondScene::ID);
    CHECK_NOTHROW(scenes.ActiveScene());

    Rect tr = SecondScene::GetTopRect();
    Point insideTopPoint(tr.GetLeft() + Random::Roll(0, tr.GetWidth()-1),
                         tr.GetTop() + Random::Roll(0, tr.GetHeight()-1));

    Rect br = SecondScene::GetBotRect();
    Point insideBotPoint(br.GetLeft() + Random::Roll(0, br.GetWidth()-1),
                         br.GetTop() + Random::Roll(0, br.GetHeight()-1));

    SUBCASE("Scene Process Input")
    {
        // Arrange
        event.mType = EventTypes::Press;
        CHECK_NOTHROW(scenes.ActiveScene());
        scenes.ActiveScene().Render(renderer);

        SUBCASE("Process input for Top elements")
        {
            // Arrange
            event.mCurrent = insideTopPoint;
            event.mPress = event.mCurrent;
            MESSAGE("Event Point:" << event.mCurrent);

            // Act
            scenes.ActiveScene().ProcessInput(event);

            // Assert
            CHECK(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
            CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());
        }
        SUBCASE("Process input for Bot elements")
        {
            // Arrange
            event.mCurrent = insideBotPoint;

            // Act
            scenes.ActiveScene().ProcessInput(event);

            // Assert
            CHECK(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());
            CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
        }
    }
    SUBCASE("Scene Render Elements")
    {
        // Arrange
        scenes.ActiveSceneAs<SecondScene>().GetTopBtn().Invalidate();
        scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().Invalidate();
        CHECK(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
        CHECK(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());

        // Act
        scenes.ActiveScene().Render(renderer);

        // Assert
        CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
        CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());
    }

    SUBCASE("Scene Bind click Callbacks")
    {
        Broker<ClickTopics> broker;
        Publisher<ClickTopics> publisher(broker);
        bool clicked = false;
        scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().OnClick() = [&publisher, &clicked](const Point&, uint32_t) {
            clicked = true;
            MESSAGE("Click detected");
            rsp::messaging::ClickedEvent click_event("Button was clicked.");
            publisher.PublishToBroker(ClickTopics::SceneChange, click_event);
        };

        // Arrange
        TestSub sub(broker);
        sub.Subscribe(ClickTopics::SceneChange);
        event.mCurrent = insideBotPoint;

        // Act
        event.mType = EventTypes::Press;
        event.mPress = event.mCurrent;
        scenes.ActiveScene().ProcessInput(event);
        event.mType = EventTypes::Lift;
        scenes.ActiveScene().ProcessInput(event);

         // Assert
        CHECK(clicked);
        CHECK(sub.calledCount == 1);
        CHECK(sub.message == "Button was clicked.");
    }
    CHECK_NOTHROW(renderer.Present());
}

TEST_SUITE_END();
