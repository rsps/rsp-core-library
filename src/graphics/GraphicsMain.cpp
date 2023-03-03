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
#include <graphics/StaticTextures.h>
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

GraphicsMain::GraphicsMain(Renderer &arRenderer, GfxEvents &arEvents, SceneMap &arScenes)
    : mrRenderer(arRenderer),
      mrEvents(arEvents),
      mrScenes(arScenes)
{
    mrRenderer.Fill(Color::None).Present();
}

GraphicsMain::~GraphicsMain()
{
}

void GraphicsMain::Run(int aMaxFPS, bool aPollTimers)
{
    GfxEvent event;
    rsp::utils::StopWatch sw;
    int64_t frame_time = 1000 / aMaxFPS;

    StaticTextures::Get().Load(mrRenderer);

    while (!mTerminated) {

        sw.Reset();

        if (aPollTimers) {
            rsp::utils::TimerQueue::Get().Poll();
        }

        // New scene requested?
        if (mNextScene) {
            mrEvents.Flush(); // New scene should not inherit un-handled touch events...
            mrScenes.SetActiveScene(mNextScene);
            mrScenes.ActiveScene().MakeTextures(mrRenderer);
            mNextScene = 0;
        }

        // New inputs?
        if (mrEvents.Poll(event)) {
            Logger::GetDefault().Debug() << "Touch Event: " << event;
            mrScenes.ActiveScene().ProcessInput(event);
        }

        bool changed = mrScenes.ActiveScene().UpdateData();
//        mrScenes.ActiveScene().Invalidate();

        // Render invalidated things
        mrScenes.ActiveScene().Render(mrRenderer);
        if (mpOverlay) {
            changed |= mpOverlay->UpdateData();
            mpOverlay->Render(mrRenderer);
        }
        if (changed) {
            mrRenderer.Present();
        }

        int64_t delay = std::max(std::int64_t(0), frame_time - sw.Elapsed<std::chrono::milliseconds>());
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        mFps = 1000 / std::max(std::int64_t(1), sw.Elapsed<std::chrono::milliseconds>());
    }
}

} // namespace rsp::graphics
