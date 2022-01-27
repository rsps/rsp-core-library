/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/Framebuffer.h"
#include "graphics/GraphicsMain.h"
#include <doctest.h>
#include <vector>

using namespace rsp::graphics;

TEST_CASE("Graphics Main Test")
{
    // Make touch area(s)
    Rect rect(Point(100, 100), Point(400, 400));
    Image img("testImages/Green.bmp", "testImages/Red.bmp", rect);
    TouchArea area(rect, img);

    // Make framebuffer
    Framebuffer fb;
    Rect screenSize(Point(0, 0), Point(fb.GetWidth(), fb.GetHeight()));

    // Make scene
    Scene myScene(screenSize);
    // Give scene touch areas
    myScene.AddArea(&area);
    // Give scene control/image children
    std::vector<Control *> images = {&img};
    myScene.SetChildren(images);

    // Make EventCreator
    EventCreator ec;

    // Make GraphicsMain
    GraphicsMain gMain(fb, ec, myScene);

    // Run?
    gMain.Run();
}