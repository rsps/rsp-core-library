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
#include "graphics/InputCreator.h"
#include <graphics/controls/SceneMap.h>
#include <messaging/Subscriber.h>
#include <messaging/Broker.h>
#include <messaging/eventTypes/ClickedEvent.h>

namespace rsp::graphics
{

class GraphicsMain : public rsp::messaging::Subscriber<rsp::messaging::ClickTopics>
{
  public:
    GraphicsMain(BufferedCanvas &aCanvas, InputCreator &aInputs, SceneMap &arScenes, rsp::messaging::Broker<rsp::messaging::ClickTopics>& arBroker);
    GraphicsMain(const GraphicsMain &) = default;
    GraphicsMain &operator=(const GraphicsMain &) = default;
    ~GraphicsMain();

    /**
     * \brief Runs the Gui loop
     */
    void Run();

    /**
     * \brief Sets Gui loop to terminate on next loop through
     */
    void Terminate() { mTerminated = true; }

    /**
     * \brief Change the current active Scene
     * \param aSceneName The name of the scene to change to
     */
    void ChangeScene(std::string aSceneName);

    /**
     * \brief Override Handle for receiving messaging events
     * \param arNewEvent Reference to the event
     */
    void HandleEvent(rsp::messaging::Event &arNewEvent) override;

  private:
    BufferedCanvas &mBufferedCanvas;
    InputCreator &mInputs;
    SceneMap &mrScenes;
    bool mTerminated = false;
    std::vector<Input> inputCache{};

    // Temp
    void PrintInput(Input aInput);
};

} // namespace rsp::graphics

#endif // GRAPHICSMAIN_H
