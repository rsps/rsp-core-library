/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <doctest.h>
#include <posix/FileSystem.h>
#include <graphics/Framebuffer.h>
#include <graphics/GraphicsMain.h>
#include "../unit/controls/scenes/Scenes.h"

using namespace rsp::graphics;

TEST_CASE("Graphics Main Test")
{
    // Make framebuffer
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    // Get screen size
    Rect screenSize(Point(0, 0), Point(fb.GetWidth(), fb.GetHeight()));

    // Make scenes
    Scenes scenes;

    // Make TouchParser
    TouchParser tp("testImages/touchTest.bin");

    GraphicsMain gfx(fb, tp, scenes);

    gfx.ChangeScene("SecondScene");

    scenes.Second().Whenclicked() = [&gfx]() {
        gfx.Terminate();
    };

    gfx.Run();
}
