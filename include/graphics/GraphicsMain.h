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

#include "graphics/BufferedCanvas.h"
#include <graphics/controls/SceneMap.h>
#include <graphics/TouchParser.h>
#include <messaging/Subscriber.h>
#include <messaging/Broker.h>

namespace rsp::graphics
{

class GraphicsMain
{
public:
    GraphicsMain(BufferedCanvas &arCanvas, TouchParser &arTouchParser, SceneMap &arScenes);
    GraphicsMain(const GraphicsMain &) = default;
    GraphicsMain &operator=(const GraphicsMain &) = default;
    ~GraphicsMain();

    /**
     * \brief Runs the Gui loop. The overall CPU usage can be reduced by setting a lower frame rate.
     * \param Maximum allowed frames per second on the GUI.
     */
    void Run(int aMaxFPS = 30);

    /**
     * \brief Sets Gui loop to terminate on next loop through
     */
    void Terminate() { mTerminated = true; }

    /**
     * \brief Change the current active Scene
     * \param aSceneName The name of the scene to change to
     */
    void ChangeScene(const std::string &arName) { mNextScene = arName; }

protected:
    BufferedCanvas &mrBufferedCanvas;
    TouchParser &mrTouchParser;
    SceneMap &mrScenes;
    bool mTerminated = false;
    std::string mNextScene{};
};

} // namespace rsp::graphics

#endif // GRAPHICSMAIN_H
