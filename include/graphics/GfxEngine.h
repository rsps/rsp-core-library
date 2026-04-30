/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_GFX_ENGINE_H
#define RSP_CORE_LIB_GRAPHICS_GFX_ENGINE_H

#include <vector>
#include "SceneMap.h"
#include <logging/LogChannel.h>
#include <messaging/EventBroker.h>
#include <utils/StopWatch.h>

namespace rsp::graphics {

class GfxEngineBase
{
public:
    explicit GfxEngineBase(int aMaxFPS = 1000);
    virtual ~GfxEngineBase() = default;

    /**
     * \brief Set the active scene. Call this from Broker event handler or other pace in sync with Iterate flow
     *
     * \param aId 32-bit id of scene
     * \return self
     */
    GfxEngineBase& SetNextScene(uint32_t aId);

    template <class E, typename = std::enable_if_t<std::is_enum_v<E>, E>>
    GfxEngineBase& SetNextScene(E e) { return SetNextScene(static_cast<uint32_t>(e)); }

    bool Iterate();

    [[nodiscard]] int GetFPS() const;

    GfxEngineBase& AddOverlay(Control &arControl);
    GfxEngineBase& ClearOverlays();

    [[nodiscard]] virtual SceneMap& GetSceneMap() = 0;
    [[nodiscard]] virtual messaging::BrokerInterface& GetEventBroker() = 0;

protected:
    int mFrameTime;
    int mFps = 0;
    uint32_t mIterations = 0;
    utils::StopWatch mStopWatch{};
    uint32_t mNextScene = 0;
    std::vector<Control*> mOverlays{};
    SceneMap::SceneNotify::Listener_t mListeners[2];
    logging::LogChannel mLogger;

    virtual void iterateTimers();
    virtual void iterateEvents();
    /**
     * @return True if data was changed
     */
    virtual bool updateData();
    virtual void render();
    virtual void updateFPS();

    void afterSceneCreated(Scene& arScene);
    void beforeSceneDestroyed(Scene& arScene);
};


template <class TSceneMap, class TBroker = messaging::EventBroker>
class GfxEngine : public GfxEngineBase
{
public:
    explicit GfxEngine(const int aMaxFPS = 1000)
        : GfxEngineBase(aMaxFPS)
    {
        mListeners[0] = mSceneMap.GetAfterCreate().Listen(std::bind(&GfxEngine::afterSceneCreated, this, std::placeholders::_1));
        mListeners[1] = mSceneMap.GetBeforeDestroy().Listen(std::bind(&GfxEngine::beforeSceneDestroyed, this, std::placeholders::_1));
    }

    messaging::BrokerInterface& GetEventBroker() override
    {
        return mBroker;
    }

    SceneMap& GetSceneMap() override
    {
        return mSceneMap;
    }

protected:
    TSceneMap mSceneMap{};
    TBroker mBroker{};
};

} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_GRAPHICS_GFX_ENGINE_H
