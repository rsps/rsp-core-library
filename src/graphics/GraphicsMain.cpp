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

namespace rsp::graphics
{

GraphicsMain::GraphicsMain(BufferedCanvas &aCanvas, EventCreator &aEvents, Scene &aScene)
    : mCanvas(aCanvas), mEvents(aEvents), mActiveScene(aScene)
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
        }
        // anything to render?
        // re-render invalidated things
        // do Idle
    }
}
} // namespace rsp::graphics