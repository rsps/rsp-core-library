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

GraphicsMain::GraphicsMain(Renderer &arRenderer, GfxInputEvents &arEvents, SceneMap &arScenes)
    : mrRenderer(arRenderer),
      mrEvents(arEvents),
      mrScenes(arScenes)
{
    mrRenderer.Fill(Color::None).Present();
}

GraphicsMain::~GraphicsMain()
{
}

GraphicsMain& GraphicsMain::ChangeScene(std::uint32_t aId)
{
    mNextScene = aId;
    return *this;
}

bool GraphicsMain::Iterate(int aMaxFPS, bool aPollTimers)
{
    rsp::utils::StopWatch sw;
    int64_t frame_time = 1000 / aMaxFPS;
    GfxEvent event;
    bool result = true;

    if (aPollTimers) {
        rsp::utils::TimerQueue::GetInstance().Poll();
    }

    // New scene requested?
    if (mNextScene) {
        mrEvents.Flush(); // New scene should not inherit un-handled touch events...
        mrScenes.SetActiveScene(mNextScene);
        mNextScene = 0;
    }

    Scene& scene  = mrScenes.ActiveScene();

    // New inputs?
    while (mrEvents.Poll(event)) {
        Logger::GetDefault().Debug() << "Touch Event: " << *event;
        scene.ProcessEvent(*event);
    }

    bool changed = scene.UpdateData();

    // Render invalidated things
    scene.Render(mrRenderer);
    if (mpOverlay) {
        changed |= mpOverlay->UpdateData();
        mpOverlay->Render(mrRenderer);
    }

    if (std::any_cast<const QuitEvent*>(event)) {
        mrRenderer.Flush();
        result = false;
    }
    else if (changed) {
        mrRenderer.Present();
    }

    int64_t delay = std::max(std::int64_t(0), frame_time - sw.Elapsed<std::chrono::milliseconds>());
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    mFps = 1000 / std::max(std::int64_t(1), sw.Elapsed<std::chrono::milliseconds>());

    return result;
}

GraphicsMain& GraphicsMain::RegisterOverlay(Control *apWidget)
{
    mpOverlay = apWidget;
    return *this;
}

} // namespace rsp::graphics
