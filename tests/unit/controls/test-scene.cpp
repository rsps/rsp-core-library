/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

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
using namespace std::chrono_literals;

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
#ifdef USE_GFX_SW
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Renderer::SetDevicePath(p.string());
#endif

    auto& renderer = Renderer::Init(480, 800);

    CHECK_NOTHROW(Scenes scenes_dummy);
    Scenes scenes;

    CHECK_NOTHROW(GfxEvent event_dummy);
    GfxEvent event;

    scenes.GetAfterCreate() = [](Scene arScene) {
        CHECK_EQ(arScene.GetId(), ID<SecondScene>());
        MESSAGE("Created Scene: " << arScene.GetName());
    };

    scenes.GetBeforeDestroy() = [](Scene &arScene) {
        MESSAGE("Destroying Scene: " << arScene.GetName());
    };

    CHECK_NOTHROW(scenes.SetActiveScene<SecondScene>());
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
        event.mTouchEvent.mType = TouchTypes::Press;
        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));

        SUBCASE("Process input for Top elements")
        {
            // Arrange
            event.mTouchEvent.mCurrent = insideTopPoint;
            event.mTouchEvent.mPress = event.mTouchEvent.mCurrent;
            MESSAGE("Event Point:" << event.mTouchEvent.mCurrent);

            // Act
            CHECK_NOTHROW(scenes.ActiveScene().ProcessInput(event));

            // Assert
            CHECK(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
            CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());
        }
        SUBCASE("Process input for Bot elements")
        {
            // Arrange
            event.mTouchEvent.mCurrent = insideBotPoint;

            // Act
            CHECK_NOTHROW(scenes.ActiveScene().ProcessInput(event));

            // Assert
            CHECK(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());
            CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
        }
    }
    SUBCASE("Scene Render Elements")
    {
        // Arrange
        CHECK_NOTHROW(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().Invalidate());
        CHECK_NOTHROW(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().Invalidate());
        CHECK(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
        CHECK(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());

        // Act
        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));

        // Assert
        CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
        CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());
    }

    SUBCASE("Scene Bind click Callbacks")
    {
        Broker<ClickTopics> broker;
        Publisher<ClickTopics> publisher(broker);
        bool clicked = false;
        scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().OnClick() = [&publisher, &clicked](const GfxEvent &arEvent, uint32_t) {
            clicked = true;
            MESSAGE("Click detected");
            rsp::messaging::ClickedEvent click_event("Button was clicked.");
            CHECK_NOTHROW(publisher.PublishToBroker(ClickTopics::SceneChange, click_event));
        };

        // Arrange
        TestSub sub(broker);
        CHECK_NOTHROW(sub.Subscribe(ClickTopics::SceneChange));
        CHECK_NOTHROW(event.mTouchEvent.mCurrent = insideBotPoint);

        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));
        CHECK_NOTHROW(renderer.Present());
//        std::this_thread::sleep_for(500ms);

        // Act
        event.mTouchEvent.mType = TouchTypes::Press;
        event.mTouchEvent.mPress = event.mTouchEvent.mCurrent;
        CHECK_NOTHROW(scenes.ActiveScene().ProcessInput(event));

        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));
        CHECK_NOTHROW(renderer.Present());
//        std::this_thread::sleep_for(500ms);

        event.mTouchEvent.mType = TouchTypes::Lift;
        CHECK_NOTHROW(scenes.ActiveScene().ProcessInput(event));

        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));
        CHECK_NOTHROW(renderer.Present());
//        std::this_thread::sleep_for(500ms);

         // Assert
        CHECK(clicked);
        CHECK(sub.calledCount == 1);
        CHECK(sub.message == "Button was clicked.");
    }
    CHECK_NOTHROW(renderer.Present());
}

TEST_SUITE_END();
