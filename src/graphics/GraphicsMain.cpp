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

GraphicsMain::GraphicsMain(BufferedCanvas &aBufferedCanvas, InputCreator &aInputs, Scene &aScene, Scene &aOtherScene)
    : mBufferedCanvas(aBufferedCanvas), mInputs(aInputs), mActiveScene(aScene), otherScene(aOtherScene)
{
    mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy);
}
GraphicsMain::~GraphicsMain()
{
}

void GraphicsMain::Run()
{
    while (!mTerminated) {
        // new inputs?
        while (mInputs.HasNewInputs()) {
            Input input = mInputs.GetInput();
            PrintInput(input); // Temp
            // invalidate stuff
            mActiveScene.ProcessInput(input);
        }
        // re-render invalidated things
        mActiveScene.Render(mBufferedCanvas);
        mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy); // Should be if Render returns true
    }
}

void GraphicsMain::ChangeScene()
{
    Scene temp = mActiveScene;
    mActiveScene = otherScene;
    otherScene = temp;
    otherScene.Invalidate();
    mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    mActiveScene.Render(mBufferedCanvas);
    mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy);
}

// Temp method
void GraphicsMain::PrintInput(Input aInput)
{
    if (aInput.type == InputType::Press) {
        std::cout << "New Press" << std::endl;
        std::cout << "X: " << aInput.x << " Y: " << aInput.y << std::endl;
    }
    if (aInput.type == InputType::Drag) {
        std::cout << "New Drag" << std::endl;
        std::cout << "X: " << aInput.x << " Y: " << aInput.y << std::endl;
    }
    if (aInput.type == InputType::Lift) {
        std::cout << "New Lift" << std::endl;
    }
}
} // namespace rsp::graphics