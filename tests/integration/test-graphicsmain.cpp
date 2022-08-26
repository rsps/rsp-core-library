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
#include <scenes/Scenes.h>
#include <TestHelpers.h>

using namespace rsp::graphics;

class TestTouchParser : public TouchParser
{
public:
    using TouchParser::TouchParser;

    void Flush() override
    {
        MESSAGE("Flushing touch queue.");
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

    // Make framebuffer
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    // Get screen size
    Rect screenSize(Point(0, 0), Point(fb.GetWidth(), fb.GetHeight()));

    // Make scenes
    Scenes scenes;

    // Make TouchParser
    TestTouchParser tp("testImages/touchTest.bin");

    GraphicsMain gfx(fb, tp, scenes);

    gfx.ChangeScene(SecondScene::ID);

    scenes.GetAfterCreate() = [&gfx](Scene *apScene) {
        if (apScene->GetId() == SecondScene::ID) {
            apScene->GetAs<SecondScene>().GetBottomBtn().GetOnClick() = [&gfx](const Point&, int) {
                gfx.Terminate();
            };
        }
    };

    MESSAGE("Running GFX loop with " << GFX_FPS << " FPS");
    gfx.Run(GFX_FPS);

    MESSAGE("TopLeft: " << scenes.ActiveSceneAs<SecondScene>().GetTopRect().GetTopLeft());
    const uint32_t cGreenColor = 0xFF24b40b;
    CHECK_EQ(fb.GetPixel(scenes.ActiveSceneAs<SecondScene>().GetTopRect().GetTopLeft()), cGreenColor);
    CHECK_EQ(fb.GetPixel(scenes.ActiveSceneAs<SecondScene>().GetBotRect().GetTopLeft()), cGreenColor);
}

