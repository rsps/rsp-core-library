/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <posix/FileSystem.h>
#include <graphics/SW/GfxHal.h>
#include <GuiHelper.h>
#include <Overlay.h>
#include <scenes/Scenes.h>
#include <TestHelpers.h>
#include <TestTouchParser.h>
#include <TestPixmap.h>
#include <TestEngine.h>
#include <utils/Timer.h>

using namespace rsp::graphics;
using namespace rsp::utils;
using namespace std::literals::chrono_literals;


TEST_CASE("GfxEngine")
{
    GuiHelper gh;

    auto& renderer = Renderer::Get();

    TestEngine gfx;

    Overlay overlay(gfx);

    CHECK_NOTHROW(gfx.AddOverlay(overlay));

    SUBCASE("Clear") {
        CHECK_NOTHROW(renderer.Fill(Color::None));
        CHECK_NOTHROW(renderer.Present());
        CHECK_NOTHROW(renderer.Fill(Color::None));
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Second Scene") {
        int topBtnClicked = 0;
        int bottomBtnLift = 0;
        std::vector<Control::TouchCallback_t::Listener_t> cb_list;
        SceneMap::SceneNotify::Listener_t store = gfx.GetSceneMap().GetAfterCreate().Listen([&](Scene &arScene) {
            CHECK_EQ(arScene.GetId(), Scenes::Second);

            CHECK_NOTHROW(gh.mTouchParser.SetEvents(SecondScene::GetTouchEvents().data(), SecondScene::GetTouchEvents().size()));

            int topBtnId = static_cast<int>(arScene.GetAs<SecondScene>().GetTopBtn().GetId());
            cb_list.push_back(arScene.GetAs<SecondScene>().GetTopBtn().OnClick().Listen([&topBtnClicked, topBtnId](const TouchEvent &arEvent, uint32_t id) {
                CHECK_EQ(id, topBtnId);
                topBtnClicked++;
            }));

            cb_list.push_back(arScene.GetAs<SecondScene>().GetBottomBtn().OnClick().Listen([](const TouchEvent &arEvent, uint32_t id) {
                FAIL("There should not be a click event from bottom button.");
            }));

            // Check for lift even though we lift outside button
            cb_list.push_back(arScene.GetAs<SecondScene>().GetBottomBtn().OnLift().Listen([&bottomBtnLift](const TouchEvent &arEvent, uint32_t id) {
                CHECK_EQ(arEvent.mCurrent, Point(310, 390));
                bottomBtnLift++;
            }));
        });

        CHECK_NOTHROW(gfx.SetNextScene(Scenes::Second));

        CHECK_NOTHROW(gfx.Run());

        const uint32_t cGreenColor = 0xFF24B40B;
        const uint32_t cRedColor = 0xFFC41616;
        Point toppoint = gfx.GetSceneMap().ActiveSceneAs<SecondScene>().GetTopRect().GetTopLeft() + Point(1,1);
        Point botpoint = gfx.GetSceneMap().ActiveSceneAs<SecondScene>().GetBotRect().GetTopLeft() + Point(1,1);
        CHECK_HEX(renderer.GetPixel(toppoint).AsUint(), cGreenColor);
        CHECK_HEX(renderer.GetPixel(botpoint).AsUint(), cGreenColor);
        CHECK_EQ(topBtnClicked, 2);
        CHECK_EQ(bottomBtnLift, 1);
//        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    SUBCASE("Input Scene") {
        CHECK_NOTHROW(gfx.SetNextScene(Scenes::Input));

        SceneMap::SceneNotify::Listener_t store = gfx.GetSceneMap().GetAfterCreate().Listen([&](Scene &arScene) {
            CHECK_EQ(arScene.GetId(), uint32_t(Scenes::Input));

            gh.mTouchParser.SetEvents(InputScene::GetTouchEvents().data(), InputScene::GetTouchEvents().size());
        });

        int progress = 0;
        Timer t1(1, 2800ms);
        t1.Callback() = [&](Timer &arTimer) {
            auto timeout = 200ms;
            InputScene &scene = gfx.GetSceneMap().ActiveSceneAs<InputScene>();
            switch (progress++) {
                case 0:
                    scene.GetLabel().SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Left);
                    break;
                case 1:
                    scene.GetLabel().SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Center);
                    break;
                case 2:
                    scene.GetLabel().SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Right);
                    break;
                case 3:
                    scene.GetLabel().SetVAlignment(Text::VAlign::Bottom).SetHAlignment(Text::HAlign::Left);
                    break;
                case 4:
                    scene.GetLabel().SetVAlignment(Text::VAlign::Bottom).SetHAlignment(Text::HAlign::Center);
                    break;
                case 5:
                    scene.GetLabel().SetVAlignment(Text::VAlign::Bottom).SetHAlignment(Text::HAlign::Right);
                    break;
                case 6:
                    scene.GetLabel().SetVAlignment(Text::VAlign::Center).SetHAlignment(Text::HAlign::Center);
                    break;
                case 7:
                    scene.GetLabel().SetFontSize(16);
                    timeout = 500ms;
                    break;
                case 8:
                    scene.GetLabel().SetFontSize(22);
                    timeout = 500ms;
                    break;
                case 9:
                    scene.GetLabel().SetFontSize(26);
                    timeout = 500ms;
                    break;
                case 10:
                    scene.GetLabel().SetFontSize(40);
                    timeout = 500ms;
                    CHECK_EQ(scene.GetLabel().GetText().GetValue(), "Hello 128€?sdgp");
                    break;
                case 11:
                    Control::SetTouchAreaColor(Color::Yellow);
                    scene.GetLabel().GetText();
                    scene.Invalidate();
                    timeout = 800ms;
                    break;
                case 12:
                    scene.GetLabel().SetCaption("æøåößđŋµÅÖ");
                    timeout = 800ms;
                    break;
                case 13:
                    Control::SetTouchAreaColor(Color::None);
                    scene.GetLabel().GetText();
                    scene.Invalidate();
                    timeout = 200ms;
                    break;
                default:
                    CHECK_EQ(scene.GetLabel().GetText().GetValue(), "æøåößđŋµÅÖ");
                    gfx.Terminate();
                    break;
            }
            CHECK_NOTHROW(arTimer.SetTimeout(timeout).Enable());
        };
        CHECK_NOTHROW(t1.Enable());

        CHECK_NOTHROW(gfx.Run());
    }

    MESSAGE("Finished with " << gfx.GetFPS() << " FPS and a maximum event delay of " << gh.mTouchParser.GetMaxDelay() << "ms");
#ifdef USE_GFX_SW
    MESSAGE("Video Memory Usage: " << sw::GfxHal::Get().GetVideoMemoryUsage());
#endif
    CHECK_NOTHROW(gfx.ClearOverlays());
}

