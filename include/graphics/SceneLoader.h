/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "graphics/controls/Scene.h"
#include "graphics/controls/scenes/firstScene.h"
#include "graphics/controls/scenes/secondScene.h"
#include <map>

namespace rsp::graphics
{

class SceneLoader
{
  public:
    SceneLoader(rsp::messaging::BrokerBase &aBroker);
    ~SceneLoader();

    Scene &GetFirstScene();
    Scene &GetScene(std::string aSceneName);

    FirstScene first;
    SecondScene second;

    std::unordered_map<std::string, Scene &> mSceneMap{};
};

} // namespace rsp::graphics
#endif // SCENELOADER_H