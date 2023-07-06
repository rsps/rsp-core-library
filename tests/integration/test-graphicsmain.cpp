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
#include <graphics/Renderer.h>
#include <graphics/GraphicsMain.h>
#include <graphics/Label.h>
#include <graphics/SW/GfxHal.h>
#include <utils/Timer.h>
#include <scenes/Scenes.h>
#include <TestHelpers.h>
#include <TestTouchParser.h>
#include <TestPixmap.h>

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
        GetStyle(States::Normal).mForegroundColor = Color::Yellow;
        GetStyle(States::Normal).mBackgroundColor = Color::Black;
        SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Left);
        SetName("Overlay");
    }

protected:
    GraphicsMain &mrGfx;
    int mIterations = 0;
    int mTotalFps = 0;
    int mMinFps = 10000000;
    int mMaxFps = 0;

    void refresh() override
    {
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
        Label::refresh();
    }
};

TEST_CASE("Graphics Main Test")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
    const char* cFontName = "Exo 2";

    CHECK_NOTHROW(Font::RegisterFont(cFontFile));
    CHECK_NOTHROW(Font::SetDefaultFont(cFontName));

    CHECK_NOTHROW(TimerQueue::CreateInstance());

    // Make framebuffer
#ifdef USE_GFX_SW
    std::filesystem::path p;
    CHECK_NOTHROW(p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"}));
    Renderer::SetDevicePath(p.string());
#endif

    CHECK_NOTHROW(Renderer::Init(480, 800));
    auto& renderer = Renderer::Get();

    // Set default scene size to screen size
    CHECK_NOTHROW(Scene::SetScreenSize(renderer.GetWidth(), renderer.GetHeight()));

    TestPixmap pix_map;

    // Make scenes
    CHECK_NOTHROW(SecondScene scn2);
    CHECK_NOTHROW(InputScene scn3);
    CHECK_NOTHROW(Scenes dummy_scenes);
    Scenes scenes;

    // Make TouchParser
    TestTouchParser tp;

    CHECK_NOTHROW(GraphicsMain dummy_gfx(renderer, tp, scenes));
    GraphicsMain gfx(renderer, tp, scenes);

    Overlay overlay(gfx);

    CHECK_NOTHROW(gfx.RegisterOverlay(&overlay));

    SUBCASE("Clear") {
        CHECK_NOTHROW(renderer.Fill(Color::None));
        CHECK_NOTHROW(renderer.Present());
        CHECK_NOTHROW(renderer.Fill(Color::None));
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Second Scene") {
        CHECK_NOTHROW(gfx.ChangeScene<SecondScene>());

        int topBtnClicked = 0;
        scenes.GetAfterCreate() = [&topBtnClicked, &tp](Scene &arScene) {
            CHECK_EQ(arScene.GetId(), rsp::utils::ID<SecondScene>());

            CHECK_NOTHROW(tp.SetEvents(SecondScene::GetTouchEvents().data(), SecondScene::GetTouchEvents().size()));

            int topBtnId = static_cast<int>(arScene.GetAs<SecondScene>().GetTopBtn().GetId());
            arScene.GetAs<SecondScene>().GetTopBtn().OnClick() = [&topBtnClicked, topBtnId](const TouchEvent &arEvent, uint32_t id) {
                CHECK_EQ(id, topBtnId);
                topBtnClicked++;
            };

            arScene.GetAs<SecondScene>().GetBottomBtn().OnClick() = [](const TouchEvent &arEvent, uint32_t id) {
                FAIL("There should not be a click event from bottom button.");
            };

            // Check for lift even though we lift outside button
            arScene.GetAs<SecondScene>().GetBottomBtn().OnLift() = [](const TouchEvent &arEvent, uint32_t id) {
                CHECK_EQ(arEvent.mCurrent, Point(310, 390));
            };
        };

        MESSAGE("Running GFX loop with " << GFX_FPS << " FPS");
        CHECK_NOTHROW(while(gfx.Iterate(1200, true)) continue;);

        const uint32_t cGreenColor = 0xFF24B40B;
        const uint32_t cRedColor = 0xFFC41616;
        Point toppoint = scenes.ActiveSceneAs<SecondScene>().GetTopRect().GetTopLeft() + Point(1,1);
        Point botpoint = scenes.ActiveSceneAs<SecondScene>().GetBotRect().GetTopLeft() + Point(1,1);
        CHECK_HEX(renderer.GetPixel(toppoint).AsUint(), cGreenColor);
        CHECK_HEX(renderer.GetPixel(botpoint).AsUint(), cGreenColor);
        CHECK_EQ(topBtnClicked, 2);
//        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    SUBCASE("Input Scene") {
        CHECK_NOTHROW(gfx.ChangeScene(Scenes::InputScene));

        scenes.GetAfterCreate() = [&tp](Scene &arScene) {
            CHECK_EQ(arScene.GetId(), uint32_t(Scenes::InputScene));

            tp.SetEvents(InputScene::GetTouchEvents().data(), InputScene::GetTouchEvents().size());
        };

        bool terminate = false;
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
                    scenes.ActiveSceneAs<InputScene>().GetLabel().GetText();
                    scenes.ActiveSceneAs<InputScene>().Invalidate();
                    timeout = 800ms;
                    break;
                case 12:
                    scenes.ActiveSceneAs<InputScene>().GetLabel().SetCaption("æøåößđŋµÅÖ");
                    timeout = 800ms;
                    break;
                case 13:
                    Control::SetTouchAreaColor(Color::None);
                    scenes.ActiveSceneAs<InputScene>().GetLabel().GetText();
                    scenes.ActiveSceneAs<InputScene>().Invalidate();
                    timeout = 200ms;
                    break;
                default:
                    CHECK_EQ(scenes.ActiveSceneAs<InputScene>().GetLabel().GetText().GetValue(), "æøåößđŋµÅÖ");
                    terminate = true;
                    break;
            }
            CHECK_NOTHROW(arTimer.SetTimeout(timeout).Enable());
        };
        CHECK_NOTHROW(t1.Enable());

        MESSAGE("Running GFX loop with " << 1000 << " FPS limitation");
        CHECK_NOTHROW(while (!terminate) { gfx.Iterate(1000, true); });
    }

#ifdef USE_GFX_SW
    MESSAGE("Video Memory Usage: " << sw::GfxHal::Get().GetVideoMemoryUsage());
#endif
    CHECK_NOTHROW(gfx.RegisterOverlay(nullptr));

    CHECK_NOTHROW(TimerQueue::DestroyInstance());
}

