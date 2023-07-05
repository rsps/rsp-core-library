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
#include <graphics/GraphicsMain.h>
#include <utils/Timer.h>
#include <scenes/Scenes.h>
#include <TestHelpers.h>
#include <TestPixmap.h>

using namespace rsp::graphics;
using namespace rsp::utils;
using namespace std::literals::chrono_literals;


TEST_CASE("Touch Events" * doctest::skip())
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
    const char* cFontName = "Exo 2";

    CHECK_NOTHROW(Font::RegisterFont(cFontFile));
    CHECK_NOTHROW(Font::SetDefaultFont(cFontName));

    CHECK_NOTHROW(TimerQueue::CreateInstance());

    CHECK_NOTHROW(Renderer::Init(480, 800));
    auto& renderer = Renderer::Get();

    // Set default scene size to screen size
    CHECK_NOTHROW(Scene::SetScreenSize(renderer.GetWidth(), renderer.GetHeight()));

    // Load GfxCache
    TestPixmap pix_map;

    // Make scenes
    CHECK_NOTHROW(InputScene scn3);
    Scenes scenes;

    CHECK_NOTHROW(GfxInputEvents::Get());

    CHECK_NOTHROW(GraphicsMain dummy_gfx(renderer, GfxInputEvents::Get(), scenes));
    GraphicsMain gfx(renderer, GfxInputEvents::Get(), scenes);


    SUBCASE("Clear") {
        CHECK_NOTHROW(renderer.Fill(Color::None));
        CHECK_NOTHROW(renderer.Present());
        CHECK_NOTHROW(renderer.Fill(Color::None));
        CHECK_NOTHROW(renderer.Present());
    }

    SUBCASE("Input Scene") {
        CHECK_NOTHROW(gfx.ChangeScene(Scenes::InputScene));

        int progress = 0;
        bool terminate = false;
        Timer t1(1, 500ms);
        t1.Callback() = [&](Timer &arTimer) {
            if (scenes.ActiveScene<InputScene>().GetLabel().GetText().GetValue() == "Quit") {
                terminate = true;
            }
            if (++progress > 200) {
                terminate = true;
            }
            CHECK_NOTHROW(arTimer.SetTimeout(500ms).Enable());
        };
        CHECK_NOTHROW(t1.Enable());

        MESSAGE("Running GFX loop with " << GFX_FPS << " FPS limitation");
        CHECK_NOTHROW(while (!terminate) { gfx.Iterate(GFX_FPS, true); } );
    }

    CHECK_NOTHROW(TimerQueue::DestroyInstance());
}

