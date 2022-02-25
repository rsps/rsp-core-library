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

#include "graphics/InputCreator.h"
#include "graphics/SceneLoader.h"
#include "graphics/messaging/Subscriber.h"
#include "graphics/primitives/BufferedCanvas.h"

namespace rsp::graphics
{

class GraphicsMain : public Subscriber
{
  public:
    GraphicsMain(BufferedCanvas &aCanvas, InputCreator &aInputs, SceneLoader &aSceneLoader);
    ~GraphicsMain();

    void Run();
    void Terminate() { mTerminated = true; }
    void ChangeScene(std::string aSceneName);
    void handleEvent(Event &newEvent) override;

  private:
    BufferedCanvas &mBufferedCanvas;
    InputCreator &mInputs;
    SceneLoader &mSceneLoader;
    Scene mActiveScene;
    bool mTerminated = false;
    std::vector<Input> inputCache{};

    // Temp
    void PrintInput(Input aInput);
};
} // namespace rsp::graphics
#endif // GRAPHICSMAIN_H