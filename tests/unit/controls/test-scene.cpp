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
#include <posix/FileSystem.h>
#include <utils/Random.h>
#include <doctest.h>

#include <scenes/Scenes.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::messaging;
using namespace rsp::utils;
using namespace std::chrono_literals;


TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Scene Test")
{
    TestLogger logger;

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
    TouchEvent event;

    int events_called = 0;

    auto ac = scenes.GetAfterCreate().Listen([&](Scene arScene) {
        CHECK_EQ(arScene.GetId(), uint32_t(Scenes::Second));
        MESSAGE("Created Scene: " << arScene.GetName());
        events_called++;
    });

    auto bd = scenes.GetBeforeDestroy().Listen([&](Scene &arScene) {
        CHECK_EQ(arScene.GetId(), uint32_t(Scenes::Second));
        MESSAGE("Destroying Scene: " << arScene.GetName());
        events_called++;
    });

    CHECK_NOTHROW(scenes.SetActiveScene(Scenes::Second));
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
        event.mType = TouchTypes::Press;
        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));

        SUBCASE("Process input for Top elements")
        {
            // Arrange
            event.mCurrent = insideTopPoint;
            event.mPress = event.mCurrent;
            MESSAGE("Event Point:" << event.mCurrent);

            // Act
            CHECK_NOTHROW(scenes.ActiveScene().ProcessEvent(event));

            // Assert
            CHECK(scenes.ActiveSceneAs<SecondScene>().GetTopBtn().IsInvalid());
            CHECK_FALSE(scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().IsInvalid());
        }
        SUBCASE("Process input for Bot elements")
        {
            // Arrange
            event.mCurrent = insideBotPoint;

            // Act
            CHECK_NOTHROW(scenes.ActiveScene().ProcessEvent(event));

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
        bool clicked = false;
        auto f1 = scenes.ActiveSceneAs<SecondScene>().GetBottomBtn().OnClick().Listen([&clicked](const TouchEvent&, uint32_t) {
            clicked = true;
            MESSAGE("Click detected");
        });

        // Arrange
        CHECK_NOTHROW(event.mCurrent = insideBotPoint);

        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));
        CHECK_NOTHROW(renderer.Present());
//        std::this_thread::sleep_for(500ms);

        // Act
        event.mType = TouchTypes::Press;
        event.mPress = event.mCurrent;
        CHECK_NOTHROW(scenes.ActiveScene().ProcessEvent(event));

        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));
        CHECK_NOTHROW(renderer.Present());
//        std::this_thread::sleep_for(500ms);

        event.mType = TouchTypes::Lift;
        CHECK_NOTHROW(scenes.ActiveScene().ProcessEvent(event));

        CHECK_NOTHROW(scenes.ActiveScene().UpdateData());
        CHECK_NOTHROW(scenes.ActiveScene().Render(renderer));
        CHECK_NOTHROW(renderer.Present());
//        std::this_thread::sleep_for(500ms);

         // Assert
        CHECK(clicked);
    }
    CHECK_NOTHROW(renderer.Present());

    CHECK_EQ(events_called, 1);
}

TEST_SUITE_END();
