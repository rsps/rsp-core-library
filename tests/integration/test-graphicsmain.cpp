/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */

#ifndef GFX_FPS
    #define GFX_FPS 100
#endif

#include <doctest.h>
#include <posix/FileSystem.h>
#include <graphics/Framebuffer.h>
#include <graphics/GraphicsMain.h>
#include <utils/Timer.h>
#include <scenes/Scenes.h>
#include <TestHelpers.h>
#include <TestTouchParser.h>

using namespace rsp::graphics;
using namespace rsp::utils;
using namespace std::literals::chrono_literals;

static TouchEvent& Touch(TouchEvent::Types aType, Point aPoint)
{
    static TouchEvent event;
    event.mType = aType;
    event.mCurrent = aPoint;
    if (aType == TouchEvent::Types::Press) {
        event.mPress = aPoint;
    }
    return event;
}

TEST_CASE("Graphics Main Test")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
    const char* cFontName = "Exo 2";

    CHECK_NOTHROW(Font::RegisterFont(cFontFile));
    CHECK_NOTHROW(Font::SetDefaultFont(cFontName));

    TimerQueue::Create();

    // Make framebuffer
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    // Set default scene size to screen size
    Scene::SetScreenSize(fb.GetWidth(), fb.GetHeight());

    // Make scenes
    CHECK_NOTHROW(SecondScene scn2);
    CHECK_NOTHROW(InputScene scn3);
    Scenes scenes;

    // Make TouchParser
    TestTouchParser tp;

    GraphicsMain gfx(fb, tp, scenes);

    SUBCASE("Clear") {
        fb.SwapBuffer(Framebuffer::SwapOperations::Clear);
        fb.SwapBuffer(Framebuffer::SwapOperations::Clear);
    }

    SUBCASE("Second Scene") {
        gfx.ChangeScene(SecondScene::ID);

        int topBtnClicked = 0;
        scenes.GetAfterCreate() = [&gfx, &topBtnClicked, &tp](Scene *apScene) {
            CHECK_EQ(apScene->GetId(), SecondScene::ID);

            tp.SetEvents(SecondScene::GetTouchEvents().data(), SecondScene::GetTouchEvents().size());

            int topBtnId = static_cast<int>(apScene->GetAs<SecondScene>().GetTopBtn().GetId());
            apScene->GetAs<SecondScene>().GetTopBtn().OnClick() = [&topBtnClicked, topBtnId](const Point& arPoint, int id) {
                CHECK_EQ(id, topBtnId);
                topBtnClicked++;
            };

            apScene->GetAs<SecondScene>().GetBottomBtn().OnClick() = [](const Point& arPoint, int id) {
                FAIL("There should not be a click event from bottom button.");
            };

            // Check for lift even though we lift outside button
            apScene->GetAs<SecondScene>().GetBottomBtn().OnLift() = [&gfx](const Point& arPoint, int id) {
                CHECK_EQ(arPoint, Point(310, 390));
                gfx.Terminate();
            };
        };

        MESSAGE("Running GFX loop with " << GFX_FPS << " FPS");
        gfx.Run(GFX_FPS, true);

        const uint32_t cGreenColor = 0xFF24b40b;
        CHECK_EQ(fb.GetPixel(scenes.ActiveSceneAs<SecondScene>().GetTopRect().GetTopLeft() + Point(1,1)), cGreenColor);
        CHECK_EQ(fb.GetPixel(scenes.ActiveSceneAs<SecondScene>().GetBotRect().GetTopLeft() + Point(1,1)), cGreenColor);
        CHECK_EQ(topBtnClicked, 2);
    }

    SUBCASE("Input Scene") {
        gfx.ChangeScene(InputScene::ID);

        scenes.GetAfterCreate() = [&tp](Scene *apScene) {
            CHECK_EQ(apScene->GetId(), InputScene::ID);

            tp.SetEvents(InputScene::GetTouchEvents().data(), InputScene::GetTouchEvents().size());
        };

        Timer t1(1, 2500ms);
        t1.Callback() = [&](Timer &arTimer) {
            gfx.Terminate();
        };
        t1.Enable();

        MESSAGE("Running GFX loop with " << GFX_FPS << " FPS");
        gfx.Run(GFX_FPS, true);
    }

    TimerQueue::Destroy();
}

