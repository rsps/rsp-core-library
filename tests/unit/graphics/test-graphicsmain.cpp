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
    MESSAGE("Start");
    // Make touch area(s)
    Rect rect(Point(100, 100), Point(400, 400));
    MESSAGE("Making Image");
    Image img("testImages/Red.bmp", "testImages/Green.bmp", rect);
    MESSAGE("Making Touch Area");
    TouchArea area(rect, img);

    MESSAGE("Init Framebufer");
    // Make framebuffer
    Framebuffer fb;
    Rect screenSize(Point(0, 0), Point(fb.GetWidth(), fb.GetHeight()));

    MESSAGE("Making Scene");
    // Make scene
    Scene myScene(screenSize);
    // Give scene touch areas
    myScene.AddArea(area);
    // Give scene control/image children
    // std::vector<Control *> images = {&img};
    myScene.AddChildren(img);
    std::cout << "Image pointer: " << &img << std::endl;

    MESSAGE("Init InputCreator");
    // Make InputCreator
    InputCreator ic;

    MESSAGE("Init Main");
    // Make GraphicsMain
    GraphicsMain gMain(fb, ic, myScene);

    // Run?
    gMain.Run();
}