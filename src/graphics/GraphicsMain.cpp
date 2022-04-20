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

void GraphicsMain::Run()
{
    TouchEvent event;

    while (!mTerminated) {
        if (!mNextScene.empty()) {
            mrScenes.SetActiveScene(mNextScene);
            mrScenes.ActiveScene().Invalidate();
            mNextScene = "";
        }

        // New inputs?
        if (mrTouchParser.Poll(event)) {
            std::cout << event << std::endl;
            mrScenes.ActiveScene().ProcessInput(event);
        }

        // Render invalidated things
        if (mrScenes.ActiveScene().Render(mrBufferedCanvas)) {
            mrBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy); // Should be if Render returns true
        }
    }
}


} // namespace rsp::graphics
