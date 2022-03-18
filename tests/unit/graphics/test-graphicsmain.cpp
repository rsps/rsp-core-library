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
#include "messaging/eventTypes/ClickedEvent.h"
#include <doctest.h>
#include <functional>
#include <posix/FileSystem.h>
#include <vector>

using namespace rsp::graphics;
using namespace rsp::messaging;

/*TEST_CASE("Graphics Main Test")
{
    MESSAGE("Start");

    MESSAGE("Init Framebufer");
    // Make framebuffer
    std::filesystem::path p = rsp::posix::FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});
    Framebuffer fb(p.empty() ? nullptr : p.string().c_str());

    // Get screen size
    Rect screenSize(Point(0, 0), Point(fb.GetWidth(), fb.GetHeight()));

    MESSAGE("Making Broker");
    Event event;
    // Make Broker
    Broker<ClickTopic> broker;

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
    gMain.SubscribeToBroker<ClickTopic>(broker, ClickTopic::SceneChange);
    std::cout << "Setup complete" << std::endl;

    // Run?
    // gMain.Run();
}*/
