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
#include <utils/Timer.h>
#include <algorithm>
#include <logging/Logger.h>

using namespace rsp::messaging;
using namespace rsp::logging;

namespace rsp::graphics
{

GraphicsMain::GraphicsMain(BufferedCanvas &arBufferedCanvas, TouchParser &arTouchParser, SceneMap &arScenes)
    : mrBufferedCanvas(arBufferedCanvas),
      mrTouchParser(arTouchParser),
      mrScenes(arScenes)
{
    mrBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    mrBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
}

GraphicsMain::~GraphicsMain()
{
}

void GraphicsMain::Run(int aMaxFPS, bool aPollTimers)
{
    TouchEvent event;
    rsp::utils::StopWatch sw;

    int64_t frame_time = 1000 / aMaxFPS;

    while (!mTerminated) {

        sw.Reset();

        if (aPollTimers) {
            rsp::utils::TimerQueue::Get().Poll();
        }

        // New scene requested?
        if (mNextScene) {
            mrTouchParser.Flush(); // New scene should not inherit un-handled touch events...
            mrScenes.SetActiveScene(mNextScene);
            mNextScene = 0;
        }

        // New inputs?
        if (mrTouchParser.Poll(event)) {
            Logger::GetDefault().Debug() << "Touch Event: " << event << std::endl;
            mrScenes.ActiveScene().ProcessInput(event);
        }

        // Render invalidated things
        if (mrScenes.ActiveScene().Render(mrBufferedCanvas)) {
            mrBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy);
        }

        int64_t delay = std::max(std::int64_t(0), frame_time - sw.Elapsed<std::chrono::milliseconds>());
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

} // namespace rsp::graphics
