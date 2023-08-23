/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_GFXENGINE_H_
#define INCLUDE_GRAPHICS_GFXENGINE_H_

#include <vector>
#include "Renderer.h"
#include "SceneMap.h"
#include <logging/LogChannel.h>
#include <messaging/EventBroker.h>
#include <utils/StopWatch.h>

namespace rsp::graphics {

class GfxEngineBase
{
public:
    GfxEngineBase(int aMaxFPS = 1000);
    virtual ~GfxEngineBase() {};

    /**
     * \brief Set the active scene. Call this from Broker event handler or other pace in sync with Iterate flow
     *
     * \param aId 32-bit id of scene
     * \return self
     */
    GfxEngineBase& SetNextScene(std::uint32_t aId);

    template <class E, typename = typename std::enable_if<std::is_enum<E>::value, E>::type>
    GfxEngineBase& SetNextScene(E e) { return SetNextScene(uint32_t(e)); }

    bool Iterate();

    int GetFPS() const;

    GfxEngineBase& AddOverlay(Control &arControl);
    GfxEngineBase& ClearOverlays();

    virtual SceneMap& GetSceneMap() = 0;
    virtual rsp::messaging::BrokerInterface& GetEventBroker() = 0;

protected:
    int mFrameTime;
    int mFps = 0;
    uint32_t mIterations = 0;
    rsp::utils::StopWatch mStopWatch{};
    uint32_t mNextScene = 0;
    std::vector<Control*> mOverlays{};
    SceneMap::SceneNotify::Listener_t mListeners[2];
    rsp::logging::LogChannel mLogger;

    virtual void iterateTimers();
    virtual void iterateEvents();
    virtual bool updateData();
    virtual void render();
    virtual void updateFPS();

    void afterSceneCreated(Scene& arScene);
    void beforeSceneDestroyed(Scene& arScene);
};


template <class TSceneMap, class TBroker = rsp::messaging::EventBroker>
class GfxEngine : public GfxEngineBase
{
public:
    GfxEngine(int aMaxFPS = 1000)
        : GfxEngineBase(aMaxFPS)
    {
        mListeners[0] = mSceneMap.GetAfterCreate().Listen(std::bind(&GfxEngine::afterSceneCreated, this, std::placeholders::_1));
        mListeners[1] = mSceneMap.GetBeforeDestroy().Listen(std::bind(&GfxEngine::beforeSceneDestroyed, this, std::placeholders::_1));
    }

    rsp::messaging::BrokerInterface& GetEventBroker() override
    {
        return mBroker;
    }

    rsp::graphics::SceneMap& GetSceneMap() override
    {
        return mSceneMap;
    }

protected:
    TSceneMap mSceneMap{};
    TBroker mBroker{};
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_GFXENGINE_H_ */
