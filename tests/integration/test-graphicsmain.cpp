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
    Timer t1(1, 100ms);

    // Make framebuffer
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    // Set default scene size to screen size
    Scene::SetScreenSize(fb.GetWidth(), fb.GetHeight());

    // Make scenes
    CHECK_NOTHROW(SecondScene scn2);
    Scenes scenes;

    // Make TouchParser
    TestTouchParser tp("testImages/touchTest.bin");

    GraphicsMain gfx(fb, tp, scenes);

    gfx.ChangeScene(SecondScene::ID);

    int progress = 0;
    t1.Callback() = [&](Timer &arTimer) {
        switch(progress++) {
            case 0:
                scenes.ActiveScene().ProcessInput(Touch(TouchEvent::Types::Press, {0, 0}));
                break;
            case 1:
                scenes.ActiveScene().ProcessInput(Touch(TouchEvent::Types::Lift, {0, 0}));
                break;
            default:
                gfx.Terminate();
                break;
        }
        arTimer.Enable();
    };

    scenes.GetAfterCreate() = [&gfx, &t1](Scene *apScene) {
        switch (apScene->GetId()) {
            case SecondScene::ID:
                apScene->GetAs<SecondScene>().GetBottomBtn().OnClick() = [&gfx](const Point&, int id) {
                    CHECK_EQ(id, SecondScene::ID);
                    gfx.ChangeScene(InputScene::ID);
                };
            break;

            case InputScene::ID:
                t1.Enable();
            break;

            default:
            break;
        }
    };

    MESSAGE("Running GFX loop with " << GFX_FPS << " FPS");
    gfx.Run(GFX_FPS, true);

    MESSAGE("TopLeft: " << scenes.ActiveSceneAs<SecondScene>().GetTopRect().GetTopLeft());
    const uint32_t cGreenColor = 0xFF24b40b;
    CHECK_EQ(fb.GetPixel(scenes.ActiveSceneAs<SecondScene>().GetTopRect().GetTopLeft()), cGreenColor);
    CHECK_EQ(fb.GetPixel(scenes.ActiveSceneAs<SecondScene>().GetBotRect().GetTopLeft()), cGreenColor);
}

