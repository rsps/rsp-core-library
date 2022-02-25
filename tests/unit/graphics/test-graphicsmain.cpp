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
#include <functional>
#include <vector>

using namespace rsp::graphics;

TEST_CASE("Graphics Main Test")
{

    MESSAGE("Start");

    MESSAGE("Init Framebufer");
    // Make framebuffer
    Framebuffer fb;
    Rect screenSize(Point(0, 0), Point(fb.GetWidth(), fb.GetHeight()));

    MESSAGE("Making Broker");
    // Make Broker
    Broker broker;

    MESSAGE("Making SceneLoader");
    // Make sceneLoader
    SceneLoader scenes(broker);

    MESSAGE("Init InputCreator");
    // Make InputCreator
    InputCreator ic;

    MESSAGE("Init Main");
    // Make GraphicsMain
    GraphicsMain gMain(fb, ic, scenes);

    // Subscribe to SceneChange ClickedEvents
    gMain.subscribeToBroker(&broker, Topic::SceneChange);
    std::cout << "Setup complete" << std::endl;

    // Run?
    gMain.Run();
}