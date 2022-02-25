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
static Rect screenSize = Rect(0, 0, 480, 800);
SceneLoader::SceneLoader(Broker &aBroker) : first(screenSize), second(screenSize)
{
    mSceneMap = {{"first", first},
                 {"second", second}};

    for (auto &scene : mSceneMap) {
        scene.second.RegisterBroker(&aBroker);
        scene.second.BindElementsToBroker();
    }
}

SceneLoader::~SceneLoader()
{
}

Scene &SceneLoader::GetFirstScene()
{
    try {
        return mSceneMap.at("first");
    } catch (const std::out_of_range &e) {
        throw std::out_of_range(std::string("No main scene found") + ": " + e.what());
    }
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
