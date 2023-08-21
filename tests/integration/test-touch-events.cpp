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
    #define GFX_FPS 60
#endif

#include <doctest.h>
#include <graphics/Renderer.h>
#include <GuiHelper.h>
#include <utils/Timer.h>
#include <scenes/Scenes.h>
#include <TestEngine.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::utils;
using namespace std::literals::chrono_literals;


TEST_CASE("Touch Events" * doctest::skip())
{
    GuiHelper gh(false);

    Control::SetTouchAreaColor(Color::Yellow);

    auto& renderer = Renderer::Get();

    // SDL Renderer creates
    CHECK_NOTHROW(GfxInputEvents::GetInstance());

    TestEngine gfx;

    SUBCASE("Clear") {
        CHECK_NOTHROW(renderer.Fill(Color::None));
        CHECK_NOTHROW(renderer.Present());
        CHECK_NOTHROW(renderer.Fill(Color::None));
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Input Scene") {
        CHECK_NOTHROW(gfx.SetNextScene(Scenes::Input));

        int progress = 0;
        Timer t1(1, 500ms);
        t1.Callback() = [&](Timer &arTimer) {
            if (gfx.GetSceneMap().ActiveSceneAs<InputScene>().GetLabel().GetText().GetValue() == "Quit") {
                gfx.Terminate();
            }
            if (++progress > 200) {
                gfx.Terminate();
            }
            CHECK_NOTHROW(arTimer.SetTimeout(500ms).Enable());
        };
        CHECK_NOTHROW(t1.Enable());

        CHECK_NOTHROW(gfx.Run());
    }

    MESSAGE("Finished with " << gfx.GetFPS() << " FPS and a maximum event delay of " << gh.mTouchParser.GetMaxDelay() << "ms");
}

