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
#include <graphics/controls/Label.h>
#include <graphics/Framebuffer.h>
#include <graphics/GraphicsMain.h>
#include <utils/Timer.h>
#include <scenes/Scenes.h>
#include <TestHelpers.h>
#include <TestTouchParser.h>

using namespace rsp::graphics;
using namespace rsp::utils;
using namespace std::literals::chrono_literals;

class Overlay : public Label
{
public:
    Overlay(GraphicsMain &arGfx)
        : mrGfx(arGfx)
    {
        SetTransparent(false);
        mText.GetFont().SetSize(12); //.SetBackgroundColor(Color::Black);
        SetArea(Rect(0, 0, 150, 15));
        GetStyle(States::normal).mForegroundColor = Color::Yellow;
        GetStyle(States::normal).mBackgroundColor = Color::Black;
        SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Left);
    }

    void UpdateData() override {
        int fps = mrGfx.GetFPS();
        mIterations++;
        mTotalFps += fps;
        if (fps && (fps < mMinFps)) {
            mMinFps = fps;
        }
        if (fps > mMaxFps) {
            mMaxFps = fps;
        }
        SetCaption("FPS: " + std::to_string(fps) + ", " + std::to_string(mTotalFps / mIterations) + ", " + std::to_string(mMinFps) + ", " + std::to_string(mMaxFps));
        refresh();
    }

protected:
    GraphicsMain &mrGfx;
    int mIterations = 0;
    int mTotalFps = 0;
    int mMinFps = 10000000;
    int mMaxFps = 0;
};

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

    Overlay overlay(gfx);

    gfx.RegisterOverlay(&overlay);

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
        gfx.Run(1000, true);

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

        int progress = 0;
        Timer t1(1, 2800ms);
        t1.Callback() = [&](Timer &arTimer) {
            auto timeout = 200ms;
            switch (progress++) {
                case 0:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Left);
                    break;
                case 1:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Center);
                    break;
                case 2:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Right);
                    break;
                case 3:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetVAlignment(Text::VAlign::Bottom).SetHAlignment(Text::HAlign::Left);
                    break;
                case 4:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetVAlignment(Text::VAlign::Bottom).SetHAlignment(Text::HAlign::Center);
                    break;
                case 5:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetVAlignment(Text::VAlign::Bottom).SetHAlignment(Text::HAlign::Right);
                    break;
                case 6:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetVAlignment(Text::VAlign::Center).SetHAlignment(Text::HAlign::Center);
                    break;
                case 7:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetFontSize(16);
                    timeout = 500ms;
                    break;
                case 8:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetFontSize(22);
                    timeout = 500ms;
                    break;
                case 9:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetFontSize(26);
                    timeout = 500ms;
                    break;
                case 10:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetFontSize(40);
                    timeout = 500ms;
                    CHECK_EQ(scenes.ActiveSceneAs<InputScene>().GetLabel().GetText().GetValue(), "Hello 128€?sdgp");
                    break;
                case 11:
                    Control::SetTouchAreaColor(Color::Yellow);
                    scenes.ActiveSceneAs<InputScene>().GetLabel().GetText().GetFont().SetBackgroundColor(Color::Silver);
                    scenes.ActiveSceneAs<InputScene>().Invalidate();
                    timeout = 800ms;
                    break;
                case 12:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetCaption("æøåößđŋµÅÖ");
                    timeout = 800ms;
                    break;
                case 13:
                    Control::SetTouchAreaColor(Color::None);
                    scenes.ActiveSceneAs<InputScene>().GetLabel().GetText().GetFont().SetBackgroundColor(Color::None);
                    scenes.ActiveSceneAs<InputScene>().Invalidate();
                    timeout = 200ms;
                    break;
                default:
                    CHECK_EQ(scenes.ActiveSceneAs<InputScene>().GetLabel().GetText().GetValue(), "æøåößđŋµÅÖ");
                    gfx.Terminate();
                    break;
            }
            arTimer.SetTimeout(timeout).Enable();
        };
        t1.Enable();

        MESSAGE("Running GFX loop with " << 1000 << " FPS");
        gfx.Run(1000, true);
    }

    gfx.RegisterOverlay(nullptr);

    TimerQueue::Destroy();
}

