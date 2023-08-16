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
#include <logging/Logger.h>
#include <chrono>
#include <thread>

using namespace rsp::logging;

namespace rsp::graphics {

GfxEngineBase::GfxEngineBase(int aMaxFPS)
    : mFrameTime(1000 / aMaxFPS),
      mLogger("Gfx")
{
}

GfxEngineBase& GfxEngineBase::SetNextScene(std::uint32_t aId)
{
    mNextScene = aId;
    return *this;
}

bool GfxEngineBase::Iterate()
{
//    mLogger.Debug() << "Iteration: " << mIterations++;

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

GfxEngineBase& GfxEngineBase::AddOverlay(Control &arControl)
{
    mOverlays.push_back(&arControl);
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
    if (mNextScene) {
        GfxInputEvents::GetInstance().Flush();
        GetSceneMap().SetActiveScene(mNextScene);
        mNextScene = 0;
    }
}

void GfxEngineBase::iterateEvents()
{
    auto &inputs = GfxInputEvents::GetInstance();
    auto &broker = GetEventBroker();
    GfxEvent event;

    // Fetch input events and move them to the event queue
    while (inputs.Poll(event)) {
        Logger::GetDefault().Debug() << "Got Input Event: " << *event;
        broker.Publish(event);
    }

    broker.ProcessEvents();
}

bool GfxEngineBase::updateData()
{
    bool changed = GetSceneMap().ActiveScene().UpdateData();

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
    GetSceneMap().ActiveScene().Render(renderer);

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
    mStopWatch.Reset();
}

void GfxEngineBase::afterSceneCreated(Scene &arScene)
{

    GetEventBroker().Subscribe(arScene);
}

void GfxEngineBase::beforeSceneDestroyed(Scene &arScene)
{
    GetEventBroker().Unsubscribe(arScene);
}

} /* namespace rsp::graphics */
