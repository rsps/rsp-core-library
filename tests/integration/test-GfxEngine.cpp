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
#include <graphics/GfxEngine.h>
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

class TestEngine : public GfxEngine<Scenes>, public rsp::messaging::SubscriberInterface
{
public:
    TestEngine() : GfxEngine<Scenes>(1000) {
        GetEventBroker().Subscribe(this);
    }

    void Run() {
        while(!mTerminated) {
            Iterate();
        }
    }

    void Terminate() {
        mTerminated = true;
    }

    bool ProcessEvent(rsp::messaging::Event &arEvent) override
    {
        if (arEvent.IsType<QuitEvent>()) {
            Terminate();
        }
        return false;
    }

protected:
    bool mTerminated = false;
};


class Overlay : public Label
{
public:
    Overlay(GfxEngineBase &arGfx)
        : mrGfx(arGfx)
    {
        SetTransparent(false);
        mText.GetFont().SetSize(12); //.SetBackgroundColor(Color::Black);
        SetArea(Rect(0, 0, 300, 15));
        GetStyle(States::Normal).mForegroundColor = Color::Yellow;
        GetStyle(States::Normal).mBackgroundColor = Color::Black;
        SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Left);
        SetName("Overlay");
    }

protected:
    GfxEngineBase &mrGfx;
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
        SetCaption("FPS cur: " + std::to_string(fps) + ", avg: " + std::to_string(mTotalFps / mIterations)
            + ", min: " + std::to_string(mMinFps) + ", max: " + std::to_string(mMaxFps) + ", cnt: " + std::to_string(mIterations));
        Label::refresh();
    }
};

TEST_CASE("GfxEngine")
{
    rsp::logging::Logger logger;
    rsp::logging::LoggerInterface::SetDefault(&logger);
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
//    CHECK_NOTHROW(Scenes dummy_scenes);

    // Make TouchParser
    TestTouchParser tp;

//    CHECK_NOTHROW(GfxEngine<Scenes> dummy_gfx(GFX_FPS));
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
        std::vector<Control::TouchCallback_t::Listener_t> cb_list;
        SceneMap::SceneNotify::Listener_t store = gfx.GetSceneMap().GetAfterCreate().Listen([&topBtnClicked, &tp, &cb_list](Scene &arScene) {
            CHECK_EQ(arScene.GetId(), Scenes::Second);

            CHECK_NOTHROW(tp.SetEvents(SecondScene::GetTouchEvents().data(), SecondScene::GetTouchEvents().size()));

            int topBtnId = static_cast<int>(arScene.GetAs<SecondScene>().GetTopBtn().GetId());
            cb_list.push_back(arScene.GetAs<SecondScene>().GetTopBtn().OnClick().Listen([&topBtnClicked, topBtnId](const TouchEvent &arEvent, uint32_t id) {
                CHECK_EQ(id, topBtnId);
                topBtnClicked++;
            }));

            cb_list.push_back(arScene.GetAs<SecondScene>().GetBottomBtn().OnClick().Listen([](const TouchEvent &arEvent, uint32_t id) {
                FAIL("There should not be a click event from bottom button.");
            }));

            // Check for lift even though we lift outside button
            cb_list.push_back(arScene.GetAs<SecondScene>().GetBottomBtn().OnLift().Listen([](const TouchEvent &arEvent, uint32_t id) {
                CHECK_EQ(arEvent.mCurrent, Point(310, 390));
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
//        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    SUBCASE("Input Scene") {
        CHECK_NOTHROW(gfx.SetNextScene(Scenes::Input));

        SceneMap::SceneNotify::Listener_t store = gfx.GetSceneMap().GetAfterCreate().Listen([&tp](Scene &arScene) {
            CHECK_EQ(arScene.GetId(), uint32_t(Scenes::Input));

            tp.SetEvents(InputScene::GetTouchEvents().data(), InputScene::GetTouchEvents().size());
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

    MESSAGE("Finished with " << gfx.GetFPS() << " FPS and a maximum event delay of " << tp.GetMaxDelay() << "ms");
#ifdef USE_GFX_SW
    MESSAGE("Video Memory Usage: " << sw::GfxHal::Get().GetVideoMemoryUsage());
#endif
    CHECK_NOTHROW(gfx.ClearOverlays());

    CHECK_NOTHROW(TimerQueue::DestroyInstance());
}

