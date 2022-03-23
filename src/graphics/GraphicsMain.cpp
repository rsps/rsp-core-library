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

GraphicsMain::GraphicsMain(BufferedCanvas &aBufferedCanvas, InputCreator &aInputs, SceneLoader &aSceneLoader, Broker<ClickTopics>& arBroker)
    : Subscriber<ClickTopics>(arBroker),
      mBufferedCanvas(aBufferedCanvas),
      mInputs(aInputs),
      mSceneLoader(aSceneLoader),
      mActiveScene(&mSceneLoader.GetFirstScene())
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
        // New inputs?
        while (mInputs.HasNewInputs()) {
            inputCache.push_back(mInputs.GetInput());
            PrintInput(inputCache.back()); // Temp
            // Invalidate stuff
            mActiveScene->ProcessInput(inputCache.back());
            if (inputCache.size() > 5) {
                // Limit of cached input reached - Force render
                mActiveScene->Render(mBufferedCanvas);
                inputCache.clear();
                mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy);
            }
        }
        // re-render invalidated things
        mActiveScene->Render(mBufferedCanvas);
        mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy); // Should be if Render returns true
    }
}

void GraphicsMain::ChangeScene(std::string aSceneName)
{
    mActiveScene->Invalidate();
    mActiveScene = &mSceneLoader.GetScene(aSceneName);
    mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Clear);
    mActiveScene->Render(mBufferedCanvas);
    mBufferedCanvas.SwapBuffer(BufferedCanvas::SwapOperations::Copy);
}

void GraphicsMain::HandleEvent(Event &newEvent)
{
    ChangeScene(newEvent.GetAs<ClickedEvent>().mMessage);
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
