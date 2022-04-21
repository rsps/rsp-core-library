/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/GraphicsMain.h>
#include <chrono>
#include <thread>
#include <utils/StopWatch.h>
#include <algorithm>
#include <logging/Logger.h>

using namespace rsp::messaging;

namespace rsp::graphics
{

GraphicsMain::GraphicsMain(BufferedCanvas &aBufferedCanvas, TouchParser &aInputs, SceneMap &arScenes)
    : mrBufferedCanvas(aBufferedCanvas),
      mrTouchParser(aInputs),
      mrScenes(arScenes)
{
    mrBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    mrBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
}

GraphicsMain::~GraphicsMain()
{
}

void GraphicsMain::Run(int aMaxFPS)
{
    TouchEvent event;
    rsp::utils::StopWatch sw;

    int64_t frame_time = 1000 / aMaxFPS;

    while (!mTerminated) {
        sw.Reset();

        if (!mNextScene.empty()) {
            mrScenes.SetActiveScene(mNextScene);
            mrScenes.ActiveScene().Invalidate();
            mNextScene = "";
        }

        // New inputs?
        if (mrTouchParser.Poll(event)) {
            DLOG(event);
            mrScenes.ActiveScene().ProcessInput(event);
        }

        // Render invalidated things
        if (mrScenes.ActiveScene().Render(mrBufferedCanvas)) {
            mrBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy); // Should be if Render returns true
        }

        int64_t delay = std::max(0l, frame_time - sw.Elapsed<std::chrono::milliseconds>());
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}


} // namespace rsp::graphics
