/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/GraphicsMain.h"
#include <chrono>
#include <thread>

namespace rsp::graphics
{

GraphicsMain::GraphicsMain(BufferedCanvas &aCanvas, EventCreator &aEvents, Scene &aScene)
    : mCanvas(aCanvas), mEvents(aEvents), mActiveScene(aScene)
{
}
GraphicsMain::~GraphicsMain()
{
}

void GraphicsMain::Run()
{
    while (!mTerminated) {
        // new events?
        if (mEvents.HasNewEvents()) {
            Event event = mEvents.GetEvent();
            // invalidate stuff
            mActiveScene.ProcessEvent(event);
            // re-render invalidated things
            mActiveScene.Render(mCanvas);
        } else {
            // do Idle
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
} // namespace rsp::graphics