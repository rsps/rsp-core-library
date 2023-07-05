/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef GRAPHICSMAIN_H
#define GRAPHICSMAIN_H

#include <graphics/GfxInputEvents.h>
#include <graphics/Renderer.h>
#include <messaging/Subscriber.h>
#include <messaging/Broker.h>
#include <graphics/SceneMap.h>

namespace rsp::graphics
{

class GraphicsMain
{
public:
    GraphicsMain(Renderer &arRenderer, GfxInputEvents &arEvents, SceneMap &arScenes);
    GraphicsMain(const GraphicsMain&) = default;
    ~GraphicsMain();

    GraphicsMain& operator=(const GraphicsMain&) = default;

    /**
     * \brief Perform a single iteration on the GUI loop
     *
     * \param aMaxFPS Maximum allowed frames per second on the GUI.
     * \param aPollTimers Set to poll the timer queue on each GUI loop iteration
     * \return False if quit event was received
     */
    bool Iterate(int aMaxFPS, bool aPollTimers);

    /**
     * \brief Change the current active Scene
     * \param aSceneName The name of the scene to change to
     * \return self
     */
    GraphicsMain& ChangeScene(std::uint32_t aId);

    template <class T>
    GraphicsMain& ChangeScene() { return ChangeScene(rsp::utils::ID<T>()); }

    template <class E, typename = typename std::enable_if<std::is_enum<E>::value, E>::type>
    GraphicsMain& ChangeScene(E e) { return ChangeScene(uint32_t(e)); }


    /**
     * \brief Register a widget that is rendered after all other controls have been rendered.
     * \param apWidget
     * \return self
     */
    GraphicsMain& RegisterOverlay(Control *apWidget);

    /**
     * \brief Get current frame rates per seconds.
     * \return integer FPS
     */
    int GetFPS() { return mFps; }

protected:
    Renderer &mrRenderer;
    GfxInputEvents &mrEvents;
    SceneMap &mrScenes;
    std::uint32_t mNextScene = 0;
    Control *mpOverlay = nullptr;
    int mFps = 0;
};

} // namespace rsp::graphics

#endif // GRAPHICSMAIN_H
