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

GraphicsMain::GraphicsMain(BufferedCanvas &aBufferedCanvas, EventCreator &aEvents, Scene &aScene)
    : mBufferedCanvas(aBufferedCanvas), mEvents(aEvents), mActiveScene(aScene)
{
}
GraphicsMain::~GraphicsMain()
{
}

void GraphicsMain::Run()
{
    while (!mTerminated) {
        // new events?
        while (mEvents.HasNewEvents()) {
            Event event = mEvents.GetEvent();
            PrintEvent(event); // Temp
            // invalidate stuff
            mActiveScene.ProcessEvent(event);
        }
        // re-render invalidated things
        mActiveScene.Render(mBufferedCanvas);
        mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy); // Should be if Render returns true
    }
}

// Temp method
void GraphicsMain::PrintEvent(Event event)
{
    if (event.type == EventType::Press) {
        std::cout << "New Press" << std::endl;
        std::cout << "X: " << event.x << " Y: " << event.y << std::endl;
    }
    if (event.type == EventType::Drag) {
        std::cout << "New Drag" << std::endl;
        std::cout << "X: " << event.x << " Y: " << event.y << std::endl;
    }
    if (event.type == EventType::Lift) {
        std::cout << "New Lift" << std::endl;
    }
}
} // namespace rsp::graphics