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
#include <messaging/EventBroker.h>
#include <utils/StopWatch.h>

namespace rsp::graphics {

class GfxEngine
{
public:
    GfxEngine(int aMaxFPS = 1000);
    virtual ~GfxEngine() {};

    GfxEngine& SetNextScene(std::uint32_t aId);

    template <class E, typename = typename std::enable_if<std::is_enum<E>::value, E>::type>
    GfxEngine& SetNextScene(E e) { return SetNextScene(uint32_t(e)); }

    bool Iterate();

    int GetFPS() const;

    GfxEngine& AddOverlay(Control *apControl);
    GfxEngine& ClearOverlays();

protected:
    int mFrameTime;
    int mFps = 0;
    rsp::utils::StopWatch mStopWatch{};
    uint32_t mNextScene = 0;
    std::vector<Control*> mOverlays{};

    virtual void iterateTimers();
    virtual void actualizeNextScene();
    virtual void iterateEvents();
    virtual bool updateData();
    virtual void render();
    virtual void updateFPS();

    virtual Renderer& getRenderer() = 0;
    virtual SceneMap& getSceneMap() = 0;
    virtual rsp::messaging::EventBroker& getEventBroker() = 0;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_GFXENGINE_H_ */
