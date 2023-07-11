/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/GfxEngine.h>
#include <graphics/Renderer.h>
#include <chrono>
#include <thread>

namespace rsp::graphics {

GfxEngineBase::GfxEngineBase(int aMaxFPS)
    : mFrameTime(1000 / aMaxFPS)
{
}

GfxEngineBase& GfxEngineBase::SetNextScene(std::uint32_t aId)
{
    mNextScene = aId;
    return *this;
}

bool GfxEngineBase::Iterate()
{
    iterateTimers();
    iterateEvents();

    actualizeNextScene();

    bool changed = updateData();
    if (changed) {
        render();
    }

    updateFPS();

    return true;
}

int GfxEngineBase::GetFPS() const
{
    return mFps;
}

GfxEngineBase& GfxEngineBase::AddOverlay(Control *apControl)
{
    mOverlays.push_back(apControl);
    return *this;
}

GfxEngineBase& GfxEngineBase::ClearOverlays()
{
    mOverlays.clear();
    return *this;
}

void GfxEngineBase::iterateTimers()
{
    rsp::utils::TimerQueue::GetInstance().Poll();
}

void GfxEngineBase::actualizeNextScene()
{
    getSceneMap().SetActiveScene(mNextScene);
    mNextScene = 0;
    GfxInputEvents::Get().Flush();
}

void GfxEngineBase::iterateEvents()
{
    auto &inputs = GfxInputEvents::Get();
    auto &broker = getEventBroker();
    GfxEvent event;

    // Fetch input events and move them to the event queue
    while (inputs.Poll(event)) {
        broker.Publish(event);
    }

    broker.ProcessEvents();
}

bool GfxEngineBase::updateData()
{
    bool changed = getSceneMap().ActiveScene().UpdateData();

    for (Control* ctrl : mOverlays) {
        if (ctrl->UpdateData()) {
            changed = true;
        }
    }
    return true;
}

void GfxEngineBase::render()
{
    auto &renderer = Renderer::Get();
    getSceneMap().ActiveScene().Render(renderer);

    for (Control* ctrl : mOverlays) {
        ctrl->Render(renderer);
    }

    renderer.Present();
}

void GfxEngineBase::updateFPS()
{
    int delay = std::max(int64_t(0), mFrameTime - mStopWatch.Elapsed<std::chrono::milliseconds>());
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    mFps = 1000 / std::max(int64_t(1), mStopWatch.Elapsed<std::chrono::milliseconds>());
}

} /* namespace rsp::graphics */
