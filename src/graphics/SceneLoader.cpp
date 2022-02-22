/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/SceneLoader.h"
#include <fstream>

namespace rsp::graphics
{

SceneLoader::SceneLoader(Broker &aBroker)
{
    for (auto &scene : mSceneMap) {
        scene.second.registerBroker(&aBroker);
    }
}

SceneLoader::~SceneLoader()
{
}

Scene &SceneLoader::GetFirstScene()
{
    return mSceneMap.begin()->second;
}

Scene &SceneLoader::GetScene(std::string aSceneName)
{
    try {
        return mSceneMap.at(aSceneName);
    } catch (const std::out_of_range &e) {
        throw std::out_of_range(std::string("Scene not found") + ": " + e.what());
    }
}

} // namespace rsp::graphics
